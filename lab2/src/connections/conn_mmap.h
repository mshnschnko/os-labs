#pragma once

#include "conn.h"

#include <string>

class ConnMmap : public Conn {
private:
    // pid_t host_pid;
    // Type type;
    size_t const size = 1024;
    std::string filepath;
public:
    void* buffer_ptr;
    // static void* host_buffer_ptr;
    // static void* client_buffer_ptr;

    ConnMmap(pid_t host_pid, Type type);
    virtual bool Open() override;
    virtual bool Read(void* buffer, size_t size) override;
    virtual bool Write(void* buffer, size_t size) override;
    virtual void Close() override;
    virtual ~ConnMmap() = default;
};