#pragma once

#include "conn.h"

#include <string>
#include <mqueue.h>

class ConnMq : public Conn {
private:
    mqd_t mqd;
    static constexpr int msg_size = 64;
public:
    ConnMq(pid_t host_pid, Type type);
    virtual bool Open() override;
    virtual bool Read(void* buffer, size_t size) override;
    virtual bool Write(void* buffer, size_t size) override;
    virtual void Close() override;
    virtual ~ConnMq() = default;
};
