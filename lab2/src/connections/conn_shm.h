#pragma once

#include "conn.h"

#include <string>

class ConnShm : public Conn {
private:
    size_t const size = 1024;
    void* buffer_ptr = nullptr;
    int fd;
public:
    ConnShm(pid_t host_pid, Type type);
    virtual bool Open() override;
    virtual bool Read(void* buffer, size_t size) override;
    virtual bool Write(void* buffer, size_t size) override;
    virtual void Close() override;
    virtual ~ConnShm() = default;
};