#include "client.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Wrong number of arguments. One argument is required: host pid.";
        exit(EXIT_FAILURE);
    }
    pid_t host_pid;
    try {
        host_pid = std::atoi(argv[1]);
    }
    catch (std::exception const &e) {
        std::cout << e.what();
        exit(EXIT_FAILURE);
    }
    Client& client = Client::GetInstance(host_pid);
    client.Run();
    return 0;
}