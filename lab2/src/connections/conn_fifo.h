#pragma once

#include "conn.h"

#include <string>

class ConnFifo : public Conn {
private:
    int fd;
public:
    ConnFifo(pid_t host_pid, Type type);
    virtual bool Open() override;
    virtual bool Read(void* buffer, size_t size) override;
    virtual bool Write(void* buffer, size_t size) override;
    virtual void Close() override;
    virtual ~ConnFifo() = default;
};