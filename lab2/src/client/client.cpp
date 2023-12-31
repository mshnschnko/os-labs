#include "client.h"

#include <iostream>
#include <syslog.h>
#include <cstdlib>
#include <fcntl.h>
#include <semaphore.h>
#include <memory>
#include <string.h>
#include <sys/types.h>

Client& Client::GetInstance(pid_t host_pid) {
    static Client client(host_pid);
    return client;
}

Client::Client(pid_t host_pid) : host_pid(host_pid), connection(Conn::GetConnection(host_pid, Conn::Type::CLIENT)) {
    openlog("client", LOG_NDELAY | LOG_PID | LOG_PERROR, LOG_USER);
    std::string sem_read_path = "/semH2C" + std::to_string(host_pid);
    sem_read = sem_open(sem_read_path.c_str(), 0);
    if (sem_read == SEM_FAILED) {
        syslog(LOG_ERR, "Error when opening semaphore. Client will terminate");
        exit(EXIT_FAILURE);
    }
    std::string sem_write_path = "/semC2H" + std::to_string(host_pid);
    sem_write = sem_open(sem_write_path.c_str(), 0);
    if (sem_write == SEM_FAILED) {
        syslog(LOG_ERR, "Error when opening semaphore. Client will terminate");
        exit(EXIT_FAILURE);
    }
    syslog(LOG_INFO, "Semaphores opened");
    srand(time(nullptr));
}

bool Client::OpenConnection() {
    time_t start = time(nullptr);
    time_t cur_time;
    bool conn_rc = false;
    syslog(LOG_INFO, "Try to open connection");
    do {
        conn_rc = connection->Open();
        cur_time = time(nullptr);
    } while (!conn_rc && difftime(start, cur_time) < connection_timeout);
    return conn_rc;
}

int Client::GenerateRandomNumber(int a, int b) {
    return (int)(rand()%(b - a + 1) + a);
}

bool Client::SendNumber() {
    int num = GenerateRandomNumber(min_num, state == GoatState::ALIVE ? max_num_for_alive : max_num_for_dead);
    syslog(LOG_INFO, "Sent number %d", num);
    bool rc = connection->Write(static_cast<void *>(&num), sizeof(num));
    sem_post(sem_write);
    return rc;
}

bool Client::GetState() {
    struct timespec ts{};
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += connection_timeout;
    if (sem_timedwait(sem_read, &ts) == -1)
        return false;
    bool rc = connection->Read(static_cast<void *>(&state), sizeof(state));
    if (rc) {
        std::string state_str = "Alive";
        if (state == GoatState::DEAD)
            state_str = "Dead";
        else if (state == GoatState::GAMESTOP)
            state_str = "Finish";
        syslog(LOG_INFO, "Got state %s", state_str.c_str());
    }
    return rc;
}

void Client::Run() {
    if (host_pid == -1) {
        syslog(LOG_ERR, "Host pid is not set. Client will terminate");
        exit(EXIT_FAILURE);
    }
    syslog(LOG_INFO, "Start client");
    if (kill(host_pid, SIGUSR1) < 0) {
        syslog(LOG_ERR, "There is no host with this pid. Client will terminate");
        exit(EXIT_FAILURE);
    }
    if (!OpenConnection()) {
        syslog(LOG_ERR, "Could not connect to host. Client will terminate");
        exit(EXIT_FAILURE);
    }
    syslog(LOG_INFO, "Game started");
    do {
        if (!SendNumber()) {
            syslog(LOG_ERR, "Error when sending number. Client will terminate");
            exit(EXIT_FAILURE);
        }
        if (!GetState()) {
            syslog(LOG_ERR, "Error when getting the state");
            exit(EXIT_FAILURE);
        }
    } while(state != GoatState::GAMESTOP);
    syslog(LOG_INFO, "Game finished");
}

Client::~Client() {
    closelog();
    connection->Close();
}
