#pragma once
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>
#include "memory_broker.h"

namespace nimby::platform::linux_os {
struct Mapping { uint64_t begin{}, end{}, offset{}; bool readable{}, executable{}; std::string path; };
struct BinaryIdentity { std::string sha256; uint64_t size{}; bool elf64{}; };
BinaryIdentity identify(const std::filesystem::path& path);
class Process {
    int pid_;
    uint64_t start_time_;
    int memory_ = -1;
    std::unique_ptr<MemoryBrokerClient> broker_;
public:
    explicit Process(int pid);
    ~Process();
    Process(const Process&) = delete;
    Process& operator=(const Process&) = delete;
    int pid() const noexcept { return pid_; }
    bool alive() const noexcept;
    std::filesystem::path executable() const;
    std::vector<Mapping> mappings() const;
    uint64_t image_base() const;
    bool read(uint64_t address, void* output, size_t size) const noexcept;
};
}
