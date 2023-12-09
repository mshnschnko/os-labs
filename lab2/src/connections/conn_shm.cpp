#include "conn_shm.h"

#include <sys/shm.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

std::unique_ptr<Conn> Conn::GetConnection(pid_t host_pid, Type type) {
    return std::make_unique<ConnShm>(host_pid, type);
}

ConnShm::ConnShm(pid_t host_pid, Type type) {
    this->host_pid = host_pid;
    this->type = type;
    filepath = "shm" + std::to_string(host_pid);
}

bool ConnShm::Open() {
    if (type == Type::HOST)
        fd = shm_open(filepath.c_str(), O_CREAT | O_RDWR | O_EXCL, 0777);
    else
        fd = shm_open(filepath.c_str(), O_RDWR, 0777);
    if (fd == -1)
        return false;
    ftruncate(fd, size);
    buffer_ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, fd, 0);
    if (buffer_ptr == MAP_FAILED) {
        close(fd);
        if (type == Type::HOST)
            shm_unlink(filepath.c_str());
        return false;
    }
    return true;
}

bool ConnShm::Read(void* buffer, size_t size) {
    if (size > this->size)
        return false;
    memcpy(buffer, buffer_ptr, size);
    return true;
}

bool ConnShm::Write(void* buffer, size_t size) {
    if (size > this->size)
        return false;
    memcpy(buffer_ptr, buffer, size);
    return true;
}

void ConnShm::Close() {
    munmap(buffer_ptr, size);
    close(fd);
    if (type == Type::HOST)
        shm_unlink(filepath.c_str());
}