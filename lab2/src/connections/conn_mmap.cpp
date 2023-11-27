#include "conn_mmap.h"

#include <sys/shm.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <iostream>

// void* ConnMmap::host_buffer_ptr = nullptr;
// void* ConnMmap::client_buffer_ptr = nullptr;

std::unique_ptr<Conn> Conn::GetConnection(pid_t host_pid, Type type) {
    return std::make_unique<ConnMmap>(host_pid, type);
}

ConnMmap::ConnMmap(pid_t host_pid, Type type) {
    this->host_pid = host_pid;
    this->type = type;
    filepath = "mmap" + std::to_string(host_pid);
}

// bool ConnMmap::Open() {
//     if (type == Type::HOST) {
//         client_buffer_ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
//         if (client_buffer_ptr == MAP_FAILED)
//             return false;
//         host_buffer_ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
//         if (host_buffer_ptr == MAP_FAILED)
//             return false;
//     }
//     return true;
// }

// bool ConnMmap::Open() {
//     if (type == Type::HOST) {
//         if (client_buffer_ptr == nullptr) {
//             client_buffer_ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
//             if (client_buffer_ptr == MAP_FAILED) {
//                 std::cout << "mmap for client_buffer_ptr" << std::endl;
//                 return false;
//             }
//         }
//         if (host_buffer_ptr == nullptr) {
//             host_buffer_ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
//             if (host_buffer_ptr == MAP_FAILED) {
//                 std::cout << "mmap for host_buffer_ptr" << std::endl;
//                 return false;
//             }
//         }
//     }
//     std::cout << "Host buffer: " << host_buffer_ptr << std::endl;
//     std::cout << "Client buffer: " << client_buffer_ptr << std::endl;
//     return true;
// }

// bool ConnMmap::Open() {
//     buffer_ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
//     if (buffer_ptr == MAP_FAILED) {
//         perror("mmap for shared_buffer_ptr");
//         return false;
//     }

//     // Добавьте отладочные выводы
//     std::cout << "Shared buffer: " << buffer_ptr << std::endl;

//     return true;
// }

// bool ConnMmap::Open() {
//     if (type == Type::HOST) {
//         std::cout << "shm_open host" << std::endl;
//         fd = shm_open(NULL, O_CREAT | O_RDWR | O_EXCL, 0777);
//         std::cout << "shm_open host success" << std::endl;
//     }
//     else {
//         std::cout << "shm_open client success" << std::endl;
//         fd = shm_open(NULL, O_RDWR, 0777);
//         std::cout << "shm_open client success" << std::endl;
//     }
//     if (fd == -1) {
//         std::cout << "fd == -1" << std::endl;
//         return false;
//     }
//     ftruncate(fd, size);
//     std::cout << "mmap" << std::endl;
//     buffer_ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
//     std::cout << "mmap success" << std::endl;
//     if (buffer_ptr == MAP_FAILED) {
//         close(fd);
//         if (type == Type::HOST)
//             shm_unlink(NULL);
//         return false;
//     }
//     return true;
// }

bool ConnMmap::Open() {
    if (type == Type::HOST) {
        std::cout << "anon mmap host" << std::endl;
        buffer_ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        std::cout << "anon mmap host success" << std::endl;
        if (buffer_ptr == MAP_FAILED)
            return false;
    }
    else {
        std::cout << "shm_open client" << std::endl;
        fd = shm_open(NULL, O_RDWR | O_CREAT, 0666);
        if (fd == -1) {
            std::cout << "fd == -1" << std::endl;
            std::cout << "ERRNO == " << errno << std::endl;
            return false;
        }
        std::cout << "shm_open client success" << std::endl;
        std::cout << "mmap" << std::endl;
        // buffer_ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        buffer_ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (buffer_ptr == MAP_FAILED) {
            std::cout << "buffer_ptr == MAP_FAILED" << std::endl;
            std::cout << "ERRNO == " << errno << std::endl;
            return false;
        }
        std::cout << "mmap success" << std::endl;
    }
    // if (fd == -1) {
    //     std::cout << "fd == -1" << std::endl;
    //     return false;
    // }
    // ftruncate(fd, size);
    // std::cout << "mmap" << std::endl;
    // buffer_ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    // std::cout << "mmap success" << std::endl;
    // if (buffer_ptr == MAP_FAILED) {
    //     close(fd);
    //     if (type == Type::HOST)
    //         shm_unlink(NULL);
    //     return false;
    // }
    return true;
}


// bool ConnMmap::Read(void* buffer, size_t size) {
//     if (size > this->size)
//         return false;
//     std::cout << "READ 1" << std::endl;
//     if (type == Type::HOST) {
//         std::cout << "READ HOST" << std::endl;
//         memcpy(buffer, client_buffer_ptr, size);
//     }
//     else {
//         std::cout << "READ CLIENT" << std::endl;
//         memcpy(buffer, host_buffer_ptr, size);
//     }
//     return true;
// }

// bool ConnMmap::Write(void* buffer, size_t size) {
//     if (size > this->size)
//         return false;
//     std::cout << "WRITE 2" << std::endl;
//     if (type == Type::HOST) {
//         std::cout << "WRITE HOST" << std::endl;
//         memcpy(host_buffer_ptr, buffer, size);
//     }
//     else {
//         std::cout << "WRITE CLIENT" << std::endl;
//         memcpy(client_buffer_ptr, buffer, size);
//         std::cout << "WRITE CLIENT SUCCESS" << std::endl;
//     }
//     return true;
// }

// bool ConnMmap::Write(void* buffer, size_t size) {
//     if (size > this->size)
//         return false;

//     if (type == Type::HOST) {
//         std::cout << "Writing to host buffer." << std::endl;
//         std::cout << "Buffer size: " << size << std::endl;

//         // Добавим проверку на nullptr перед использованием указателей
//         if (host_buffer_ptr != nullptr && buffer != nullptr) {
//             memcpy(host_buffer_ptr, buffer, size);
//             std::cout << "Write successful." << std::endl;
//             return true;
//         } else {
//             std::cerr << "Error: host_buffer_ptr or buffer is nullptr." << std::endl;
//             return false;
//         }
//     } else {
//         std::cout << "Writing to client buffer." << std::endl;
//         std::cout << "Buffer size: " << size << std::endl;

//         // Добавим проверку на nullptr перед использованием указателей
//         if (client_buffer_ptr != nullptr && buffer != nullptr) {
//             memcpy(client_buffer_ptr, buffer, size);
//             std::cout << "Write successful." << std::endl;
//             return true;
//         } else {
//             std::cerr << "buffer is nullptr " << (buffer == nullptr) << std::endl;
//             std::cerr << "client_buffer_ptr is nullptr " << (client_buffer_ptr == nullptr) << std::endl;
//             std::cerr << "Error: client_buffer_ptr or buffer is nullptr." << std::endl;
//             return false;
//         }
//     }
// }

bool ConnMmap::Read(void* buffer, size_t size) {
    if (size > this->size)
        return false;
    
    memcpy(buffer, buffer_ptr, size);
    return true;
}

bool ConnMmap::Write(void* buffer, size_t size) {
    if (size > this->size)
        return false;
    
    memcpy(buffer_ptr, buffer, size);
    return true;
}



// void ConnMmap::Close() {
//     return;
//     // munmap(host_buffer_ptr, size);
//     // munmap(client_buffer_ptr, size);
// }

void ConnMmap::Close() {
    if (type == Type::HOST) {
        munmap(buffer_ptr, size);
        shm_unlink("");
        // int ret2 = munmap(client_buffer_ptr, size);
        // if (ret1 == -1 || ret2 == -1) {
        //     std::cout << "munmap" << std::endl;
            // Возможно, следует добавить дополнительный код для обработки ошибок munmap
        // }
    }
}