#pragma once

#include "../goat_state.h"
#include "../connections/conn.h"

#include <sys/types.h>
#include <memory>
#include <semaphore.h>

class Client {
private:
    pid_t host_pid = -1;
    std::unique_ptr<Conn> connection;
    
    static constexpr int min_num = 1;
    static constexpr int max_num_for_alive = 100;
    static constexpr int max_num_for_dead = 50;
    static constexpr int connection_timeout = 5;

    sem_t* sem_read;
    sem_t* sem_write;
    GoatState state = GoatState::ALIVE;

    Client() = default;
    Client(pid_t host_pid);
    
    bool OpenConnection();
    bool SendNumber();
    bool GetState();
    int GenerateRandomNumber(int a, int b);

    // static void signal_handler(int signum);
    
public:
    static Client& GetInstance(pid_t host_pid);
    void SetHostPid(pid_t host_pid);
    void Run();
    ~Client();
};