#pragma once

#include <sys/types.h>
#include <memory>
#include <string>

class Conn {
public:
    enum class Type {
        HOST,
        CLIENT
    };
    static std::unique_ptr<Conn> GetConnection(pid_t host_pid, Type type);
    virtual bool Open() = 0;
    virtual bool Read(void* buffer, size_t size) = 0;
    virtual bool Write(void* buffer, size_t size) = 0;
    virtual void Close() = 0;
    virtual ~Conn() = default;
protected:
    pid_t host_pid;
    Type type;
    std::string filepath;
};