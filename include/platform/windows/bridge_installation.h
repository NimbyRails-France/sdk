#pragma once
#include <nimby/detail/observation.h>
#include <windows.h>
#include <cwchar>

namespace nimby::platform::windows {
// Each bridge links its own MinHook instance. Its private lock cannot protect
// another DLL's Freeze/SuspendThread pass. A named, process-scoped mutex joins
// these independent modules without a shared C++ runtime or an SDK dependency.
// Never wait here: an existing installation may temporarily suspend this
// caller. Nested loader -> bootstrap calls on the same thread are reentrant.
class BridgeInstallation {
public:
    BridgeInstallation() noexcept {
        wchar_t name[96]{};
        std::swprintf(name,sizeof(name)/sizeof(*name),L"Local\\NimbyRailsFrance.BridgeInstallation.%lu",
            static_cast<unsigned long>(GetCurrentProcessId()));
        mutex_=CreateMutexW(nullptr,FALSE,name);
        if(!mutex_)return;
        const auto wait=WaitForSingleObject(mutex_,0);
        if(wait==WAIT_OBJECT_0||wait==WAIT_ABANDONED)status_=NIMBY_OK;
        else if(wait==WAIT_TIMEOUT)status_=NIMBY_RESOURCE_LIMIT;
    }
    ~BridgeInstallation(){if(status_==NIMBY_OK)ReleaseMutex(mutex_);if(mutex_)CloseHandle(mutex_);}
    BridgeInstallation(const BridgeInstallation&)=delete;
    BridgeInstallation& operator=(const BridgeInstallation&)=delete;
    explicit operator bool() const noexcept{return status_==NIMBY_OK;}
    uint32_t status() const noexcept{return status_;}
private:
    HANDLE mutex_{};
    uint32_t status_=NIMBY_IO_ERROR;
};
}
