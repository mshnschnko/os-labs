#pragma once

#include "conn.h"

#include <string>

class ConnMmap : public Conn {
private:
    size_t const size = 1024;
    std::string filepath = "mmap";
    void* buffer_ptr;
public:

    ConnMmap(pid_t host_pid, Type type);
    virtual bool Open() override;
    virtual bool Read(void* buffer, size_t size) override;
    virtual bool Write(void* buffer, size_t size) override;
    virtual void Close() override;
    virtual ~ConnMmap() = default;
};