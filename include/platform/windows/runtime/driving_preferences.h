#pragma once
#include <windows.h>
#include <atomic>
#include <cstdint>
#include <cwchar>
#include <iterator>

namespace nimby::windows::automatic {
inline constexpr int32_t defaultStopBeforeSignalMeters=3;
inline constexpr int32_t maximumStopBeforeSignalMeters=100;

// Private resident-SDK preferences, shared by its in-process bridge DLLs.
// Only primitive Windows LONGs cross the mapping boundary. No public mod
// capability, C++ pointer, file, simulation clock or world identity is stored.
class DrivingPreferences {
    struct Storage { LONG initialized,value; };
    static_assert(sizeof(Storage)==8&&sizeof(LONG)==4);
    static_assert(std::atomic_ref<LONG>::is_always_lock_free);
    HANDLE mapping_=nullptr;
    Storage* storage_=nullptr;
    static std::atomic_ref<LONG> atomic(LONG& value) noexcept { return std::atomic_ref<LONG>(value); }
    void close() noexcept {
        if(storage_)UnmapViewOfFile(storage_);
        if(mapping_)CloseHandle(mapping_);
        storage_=nullptr;mapping_=nullptr;
    }
public:
    DrivingPreferences()=default;
    DrivingPreferences(const DrivingPreferences&)=delete;
    DrivingPreferences& operator=(const DrivingPreferences&)=delete;
    ~DrivingPreferences(){close();}

    // Bootstrap only: create/attach once and retain the view. A rare concurrent
    // initializer is given a bounded opportunity to publish; failure leaves a
    // safe default and may be retried by bootstrap, never by native integration.
    bool open() noexcept {
        if(storage_)return ready();
        wchar_t name[80]{};
        std::swprintf(name,std::size(name),L"Local\\NRFDrivingPreferences-v1-%lu",GetCurrentProcessId());
        mapping_=CreateFileMappingW(INVALID_HANDLE_VALUE,nullptr,PAGE_READWRITE,0,sizeof(Storage),name);
        if(!mapping_)return false;
        storage_=static_cast<Storage*>(MapViewOfFile(mapping_,FILE_MAP_ALL_ACCESS,0,0,sizeof(Storage)));
        if(!storage_){close();return false;}
        LONG empty=0;
        if(atomic(storage_->initialized).compare_exchange_strong(empty,-1,std::memory_order_acq_rel)){
            atomic(storage_->value).store(defaultStopBeforeSignalMeters,std::memory_order_relaxed);
            atomic(storage_->initialized).store(1,std::memory_order_release);
            return true;
        }
        for(unsigned attempt=0;attempt<64;++attempt){if(ready())return true;SwitchToThread();}
        close();return false;
    }
    bool ready() const noexcept {
        return storage_&&atomic(storage_->initialized).load(std::memory_order_acquire)==1;
    }
    // Two lock-free loads, no handle lookup, syscall, allocation or disk access.
    int32_t get() const noexcept {
        if(!ready())return defaultStopBeforeSignalMeters;
        const auto value=atomic(storage_->value).load(std::memory_order_acquire);
        return value>=0&&value<=maximumStopBeforeSignalMeters?value:defaultStopBeforeSignalMeters;
    }
    // Called only after the resident options registry accepted this value.
    bool set(int32_t value) noexcept {
        if(value<0||value>maximumStopBeforeSignalMeters||!ready())return false;
        atomic(storage_->value).store(value,std::memory_order_release);return true;
    }
};
} // namespace nimby::windows::automatic
