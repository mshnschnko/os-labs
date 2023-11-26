#include "host.h"

#include <syslog.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <csignal>
#include <memory>
#include <string.h>
#include <string>
#include <iostream>
#include <thread>
#include <chrono>
#include <future>


Host& Host::GetInstance() {
    static Host host;
    return host;
}

void Host::SignalHandler(int signum, siginfo_t *si, void* data) {
    Host& host = Host::GetInstance();
    if (signum == SIGTERM)
        host.Terminate(EXIT_SUCCESS);
    else if (signum == SIGUSR1)
        if (host.client_pid == -1)
            host.Connect(si->si_pid);
}

Host::Host() : connection(Conn::GetConnection(pid, Conn::Type::HOST)) {
    struct sigaction sig{};
    memset(&sig, 0, sizeof(sig));
    sig.sa_sigaction = SignalHandler;
    sig.sa_flags = SA_SIGINFO;
    sigaction(SIGTERM, &sig, nullptr);
    sigaction(SIGUSR1, &sig, nullptr);
    openlog("host", LOG_NDELAY | LOG_PID | LOG_PERROR, LOG_USER);

    std::string sem_read_path = "/semC2H" + std::to_string(pid);
    sem_read = sem_open(sem_read_path.c_str(), O_CREAT | O_EXCL, 0777, 0);
    if (sem_read == SEM_FAILED) {
        syslog(LOG_ERR, "Error when opening semaphore. Host will terminate");
        exit(EXIT_FAILURE);
    }

    std::string sem_write_path = "/semH2C" + std::to_string(pid);
    sem_write = sem_open(sem_write_path.c_str(), O_CREAT | O_EXCL, 0777, 0);
    if (sem_read == SEM_FAILED) {
        syslog(LOG_ERR, "Error when opening semaphore. Host will terminate");
        exit(EXIT_FAILURE);
    }
    syslog(LOG_INFO, "Semaphores opened");
    srand(time(nullptr));
}

void Host::Terminate(int status_code) {
    syslog(LOG_INFO, "Host terminated");
    exit(status_code);
}

pid_t Host::GetPid() {
    return pid;
}

void Host::Connect(pid_t client_pid) {
    this->client_pid = client_pid;
    if (!connection->Open()) {
        syslog(LOG_ERR, "Could not connect to client. Host will terminate");
        std::cout << "Could not connect to client. Host will terminate" << std::endl;
        Terminate(EXIT_FAILURE);
    }
    std::cout << "Client connected" << std::endl;
    StartGame();
}

void Host::KillClient() {
    if (client_pid != -1) {
        syslog(LOG_INFO, "Client killed");
        kill(client_pid, SIGTERM);
    }
}

void Host::StartGame() {
    bool dead_prev_round = false;
    int round_number = 1;
    // if (future_input.wait_for(std::chrono::seconds(0)) == std::future_status::ready) // при коннекте нового клиента старт гейм запускает еще один поток ввода
        future_input = std::async(async_input);
    do {
        dead_prev_round = state == GoatState::DEAD;
        if (!StartRound(round_number++)) {
            std::cout << "Connection lost. Game finished" << std::endl;
            Terminate(EXIT_FAILURE);
        }
        std::cout << "PREV = " << (dead_prev_round) << " CUR = " << (state == GoatState::ALIVE ? "alive" : "dead") << std::endl;
    } while (!dead_prev_round || state != GoatState::DEAD);
    state = GoatState::GAMESTOP;
    SendState(state);
    std::cout << "Game over" << std::endl;
    KillClient();
    connection->Close();
    client_pid = -1;
    state = GoatState::ALIVE;
}

bool Host::GetClientNumber(int* num) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += enter_timeout;
    if (sem_timedwait(sem_read, &ts) == -1)
        return false;
    return connection->Read(num, sizeof(num));
}

bool Host::SendState(GoatState state) {
    bool rc = connection->Write(&state, sizeof(state));
    sem_post(sem_write);
    return rc;
}

bool Host::StartRound(int round_number) {
    // static int round_number = 1;
    std::cout << "Round №" << round_number << std::endl << "Enter number:" << std::endl;
    round_number++;
    int host_num = (rand()%(max_number - min_number + 1) + min_number);
    // std::future<int> future = std::async([] { int host_num = -1; std::cin >> host_num; return host_num; });
    if (future_input.wait_for(std::chrono::seconds(enter_timeout)) == std::future_status::ready) {
        int tmp_result = future_input.get();
        if (tmp_result >= min_number && tmp_result <= max_number)
            host_num = tmp_result;
        future_input = std::async(async_input);
    }
    // if (host_num == -1 || host_num < min_number || host_num > max_number)
    //     host_num = (rand()%(max_number - min_number + 1) + min_number);
    int client_num;
    if (!GetClientNumber(&client_num))
        return false;
    std::string message = state == GoatState::ALIVE ? "is alive" : "is dead";
    if (state == GoatState::ALIVE && abs(client_num - host_num) > variance_for_alive) {
        state = GoatState::DEAD;
        message = "is dead";
    }
    else if (state == GoatState::DEAD && abs(client_num - host_num) <= variance_for_dead) {
        state = GoatState::ALIVE;
        message = "was resurrected";
    }
    if (!SendState(state))
        return false;
    std::cout << "Wolf's number is " << host_num << std::endl;
    std::cout << "Goat's number is " << client_num << std::endl;
    std::cout << "Goat " << message << std::endl << std::endl << std::endl;
    return true;
    // std::thread input_number([&host_num] { std::cin >> host_num; });
    // std::this_thread::sleep_for(std::chrono::seconds(enter_timeout));
    // input_number.join();
    // if (host_num == -1) {
    //     host_num = (rand()%(max_number - min_number + 1) + min_number);
    // }
}

Host::~Host() {
    // connection->Close();
    sem_close(sem_read);
    sem_close(sem_write);
    closelog();
}

// int main(int argc, char* argv[]) {
//     Host::GetInstance(); //.StartGame();
//     while (true);
//     return 0;
// }