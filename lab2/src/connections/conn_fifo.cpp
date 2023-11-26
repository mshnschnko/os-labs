#include "conn_fifo.h"

#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>

std::unique_ptr<Conn> Conn::GetConnection(pid_t host_pid, Type type) {
    return std::make_unique<ConnFifo>(host_pid, type);
}

ConnFifo::ConnFifo(pid_t host_pid, Type type) { // : host_pid(host_pid), type(type) {
    this->host_pid = host_pid;
    this->type = type;
    filepath = "/tmp/fifo" + std::to_string(host_pid);
}

bool ConnFifo::Open() {
    if (type == Type::HOST && access(filepath.c_str(), F_OK) == -1 && mkfifo(filepath.c_str(), 0777) == -1) {
        std::cout << "MKFIFO ERROR = " << errno << std::endl;
        return false;
    }
    fd = open(filepath.c_str(), O_RDWR);
    return fd != -1;
}

bool ConnFifo::Read(void* buffer, size_t size) {
    return read(fd, buffer, size) >= 0;
}

bool ConnFifo::Write(void* buffer, size_t size) {
    return write(fd, buffer, size) >= 0;
}

void ConnFifo::Close() {
    close(fd);
}