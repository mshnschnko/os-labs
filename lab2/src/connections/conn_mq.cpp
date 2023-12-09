#include "conn_mq.h"


std::unique_ptr<Conn> Conn::GetConnection(pid_t host_pid, Type type) {
    return std::make_unique<ConnMq>(host_pid, type);
}

ConnMq::ConnMq(pid_t host_pid, Type type) {
    this->host_pid = host_pid;
    this->type = type;
    filepath = "/mq" + std::to_string(host_pid);
}

bool ConnMq::Open() {
    if (type == Type::HOST) {
        struct mq_attr attr;
        attr.mq_flags = 0;
        attr.mq_maxmsg = 1;
        attr.mq_msgsize = msg_size;
        attr.mq_curmsgs = 0;
        mqd = mq_open(filepath.c_str(), O_CREAT | O_RDWR, 0777, &attr);
    }
    else
        mqd = mq_open(filepath.c_str(), O_RDWR);
    return mqd != -1;
}

bool ConnMq::Read(void* buffer, size_t size) {
    return mq_receive(mqd, static_cast<char *>(buffer), msg_size, nullptr) != -1;
}

bool ConnMq::Write(void* buffer, size_t size) {
    return mq_send(mqd, static_cast<char const *>(buffer), size, 0) != -1;
}

void ConnMq::Close() {
    mq_close(mqd);
    mq_unlink(filepath.c_str());
}