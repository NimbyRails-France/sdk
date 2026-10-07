#pragma once
#include <platform/windows/mod_host_protocol.h>
#include <windows.h>
#include <filesystem>
#include <memory>
#include <string>

namespace nimby::mod_host {
// Private parent/child mailbox revision. Version 2 uses awake-time watchdog
// stamps; the public mod entry protocol remains V1 and requires no rebuild.
inline constexpr uint32_t channelProtocol=2;
struct WorkSlot {
    volatile LONG thread;
    alignas(8) volatile LONG64 since;
};
struct Shared {
    uint32_t version, size, targetPid;
    volatile LONG phase; // starting, callback, idle, stopping
    alignas(8) volatile LONG64 phaseSince;
    volatile LONG ready, requestState;
    WorkSlot work[64];
    uint32_t operation, inputSize, capacity, outputSize, result;
    std::array<uint64_t,8> args;
    alignas(8) uint8_t data[payloadLimit];
};
struct LaunchOptions {
    uint32_t startupTimeoutMs=15000, callbackTimeoutMs=5000, shutdownTimeoutMs=2000;
    size_t memoryLimit=1024ull*1024*1024;
    uint32_t targetPid=0;
    uint32_t cpuRate=0; // 1/10000 of machine CPU; zero selects the standalone one-core cap.
    uint32_t rpcCallsPerSecond=1500, rpcCallBurst=256;
    uint64_t rpcBytesPerSecond=32ull*1024*1024;
    double brokerCpuFraction=0.1; // CPU seconds per wall second for this supervisor.
};
// Compute fixed, equal reservations before starting any member of a batch.
// Available memory includes the smaller of physical and commit availability.
// No live worker can borrow another worker's unused CPU/RPC reservation.
bool planResources(size_t count,uint32_t processors,uint64_t totalPhysical,
                   uint64_t availableMemory,LaunchOptions& options) noexcept;
// The supervisor executes SDK code only. The mod's loader, static initializers,
// Kotlin runtime and callbacks all live in the separate child process.
class Worker {
public:
    Worker(const std::filesystem::path& executable,const std::filesystem::path& library,
           LaunchOptions options={});
    ~Worker();
    Worker(const Worker&)=delete;
    Worker& operator=(const Worker&)=delete;
    void requestStop() noexcept;
    void join();
    uint32_t pid() const noexcept;
    bool finished() const noexcept;
    std::string outcome() const;
private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
// Child initialization is separate from DLL entry, before loading any mod.
uint32_t attach(HANDLE mapping,HANDLE request,HANDLE reply,HANDLE stop,HANDLE action,uint32_t targetPid);
int runChild(int argc,wchar_t** argv);
bool isChildOf(uint32_t workerPid,uint32_t gamePid) noexcept;
}

extern "C" NIMBY_API uint32_t __cdecl NimbyInternal_StartModHosts(const wchar_t* directory) noexcept;
extern "C" NIMBY_API uint32_t __cdecl NimbyInternal_StopModHosts() noexcept;
extern "C" NIMBY_API int __cdecl NimbyInternal_ModHostMain(int argc,wchar_t** argv) noexcept;
