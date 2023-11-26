#pragma once

#include "conn.h"

#include <string>

class ConnFifo : public Conn {
private:
    // pid_t host_pid;
    // Type type;
    int fd;
    std::string filepath;
public:
    ConnFifo(pid_t host_pid, Type type);
    virtual bool Open() override;
    virtual bool Read(void* buffer, size_t size) override;
    virtual bool Write(void* buffer, size_t size) override;
    virtual void Close() override;
    ~ConnFifo() = default;
};