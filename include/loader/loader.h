#pragma once
#include <windows.h>
#include <nimby/sdk.h>
#include <cstdint>
#include <set>
#include <string>
#include <vector>

namespace nimby::loader {
struct Event {
    DWORD pid{};
    bool success{};
    std::string message;
};
// Internal loader API. Production main supplies only the recognized game hash.
// Tests supply the exact hash/path of an owned fixture executable.
class Monitor final {
public:
    Monitor(std::wstring executable, std::string expected_sha256, std::wstring sdk);
    ~Monitor();
    Monitor(const Monitor&) = delete;
    Monitor& operator=(const Monitor&) = delete;
    std::vector<Event> poll();
    const std::wstring& executable() const noexcept { return executable_; }
private:
    std::wstring executable_, sdk_;
    std::string expected_sha256_;
    HMODULE local_sdk_{};
    HANDLE sdk_file_ = INVALID_HANDLE_VALUE;
    uintptr_t bootstrap_rva_{};
    // PID + creation time distinguishes process instances, including reused PIDs.
    std::set<std::pair<DWORD, uint64_t>> attempted_;
    Event attach(DWORD pid, uint64_t creation);
};
std::wstring canonical_file(const std::wstring& path);
std::wstring executable_path();
}
