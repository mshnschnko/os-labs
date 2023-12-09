#pragma once

#include "../goat_state.h"
#include "../connections/conn.h"

#include <sys/types.h>
#include <unistd.h>
#include <memory>
#include <signal.h>
#include <semaphore.h>
#include <future>
#include <functional>
#include <iostream>

class Host {
private:
    pid_t pid = getpid();
    pid_t client_pid = -1;
    std::unique_ptr<Conn> connection;
    GoatState state = GoatState::ALIVE;
    sem_t* sem_read;
    sem_t* sem_write;

    static constexpr int min_number = 1;
    static constexpr int max_number = 100;
    static constexpr int variance_for_alive = 70;
    static constexpr int variance_for_dead = 20;
    static constexpr int enter_timeout = 3;

    std::future<int> future_input;
    std::function<int()> async_input = []() -> int { int host_num; std::cin >> host_num; return host_num; };
    bool future_has_been_activated = false;

    Host();
    void Connect(pid_t client_pid);
    bool StartRound(int round_number);
    void KillClient();
    void Terminate(int status_code);
    static void SignalHandler(int signum, siginfo_t *si, void* data);
    bool GetClientNumber(int* client_num);
    bool SendState(GoatState);

public:
    static Host& GetInstance();
    pid_t GetPid();
    void StartGame();
    ~Host();
};