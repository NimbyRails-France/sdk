#pragma once
#include <windows.h>
#include <utility>

namespace nimby::platform::windows {
// Owns CloseHandle-compatible objects only (never HMODULE, HKEY or find handles).
// Both Win32 failure sentinels are accepted. This makes partial initialization
// safe: every early return and exception releases the acquired resource.
class UniqueHandle {
    HANDLE value_ = nullptr;
public:
    UniqueHandle() noexcept = default;
    explicit UniqueHandle(HANDLE value) noexcept : value_(value) {}
    ~UniqueHandle() { reset(); }
    UniqueHandle(const UniqueHandle&) = delete;
    UniqueHandle& operator=(const UniqueHandle&) = delete;
    UniqueHandle(UniqueHandle&& other) noexcept : value_(other.release()) {}
    UniqueHandle& operator=(UniqueHandle&& other) noexcept {
        if(this != &other) reset(other.release());
        return *this;
    }
    HANDLE get() const noexcept { return value_; }
    explicit operator bool() const noexcept {
        return value_ && value_ != INVALID_HANDLE_VALUE;
    }
    HANDLE release() noexcept { return std::exchange(value_, nullptr); }
    void reset(HANDLE value=nullptr) noexcept {
        if(value_ == value) return;
        if(*this) CloseHandle(value_);
        value_ = value;
    }
};
}
