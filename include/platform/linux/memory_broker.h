#pragma once
#include <cstddef>
#include <cstdint>
#include <memory>

namespace nimby::platform::linux_os {
// The game explicitly starts this read-only endpoint. Unix peer credentials
// restrict it to the game's own user; no ptrace policy is relaxed.
class MemoryBroker {
    struct Impl;
    std::unique_ptr<Impl> impl_;
public:
    MemoryBroker();
    ~MemoryBroker();
    MemoryBroker(const MemoryBroker&)=delete;
    MemoryBroker& operator=(const MemoryBroker&)=delete;
};
class MemoryBrokerClient {
    int socket_=-1;
    int memory_=-1;
public:
    explicit MemoryBrokerClient(int pid, uint64_t startTime);
    ~MemoryBrokerClient();
    MemoryBrokerClient(const MemoryBrokerClient&)=delete;
    MemoryBrokerClient& operator=(const MemoryBrokerClient&)=delete;
    bool read(uint64_t address,void* output,size_t size) noexcept;
};
}
