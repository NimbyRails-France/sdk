#include <platform/windows/bridge_installation.h>
#include <stdexcept>

// Built into two independent DLLs, like two bridges with static MinHook copies.
extern "C" __declspec(dllexport) uint32_t __cdecl Fixture_Bootstrap(HANDLE entered,HANDLE release,uint32_t mode) noexcept {
    try {
        nimby::platform::windows::BridgeInstallation installation;
        if(!installation)return installation.status();
        if(entered)SetEvent(entered);
        if(mode==1&&WaitForSingleObject(release,3000)!=WAIT_OBJECT_0)return NIMBY_IO_ERROR;
        if(mode==2)throw std::runtime_error("failed installation");
        return NIMBY_OK;
    }catch(...){return NIMBY_INTERNAL_ERROR;}
}
