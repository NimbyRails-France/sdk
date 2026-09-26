#include "platform/windows/unique_handle.h"
#include <nimby/detail/observation.h>
#include <cassert>

int main() {
    using nimby::platform::windows::UniqueHandle;
    {
        UniqueHandle first(CreateEventW(nullptr,TRUE,FALSE,nullptr));
        assert(first);
        const auto raw=first.get();
        UniqueHandle second(std::move(first));
        assert(!first && second.get()==raw && SetEvent(second.get()));
        first=std::move(second);
        assert(!second && WaitForSingleObject(first.get(),0)==WAIT_OBJECT_0);
        first.reset(first.get()); // Resetting to oneself must not close the handle.
        assert(WaitForSingleObject(first.get(),0)==WAIT_OBJECT_0);
    }
    UniqueHandle invalid(INVALID_HANDLE_VALUE);
    assert(!invalid);
    // Warm DLL/runtime initialization before counting OS handles. The test host
    // is not a game binary: each failed open must release its process reference.
    NimbySession session{};
    assert(NimbyInternal_OpenProcess(NIMBY_OBSERVATION_ABI_VERSION,GetCurrentProcessId(),&session)==NIMBY_UNSUPPORTED_GAME);
    DWORD before{},after{};
    assert(GetProcessHandleCount(GetCurrentProcess(),&before));
    for(unsigned i=0;i<32;++i) {
        session=99;
        assert(NimbyInternal_OpenProcess(NIMBY_OBSERVATION_ABI_VERSION,GetCurrentProcessId(),&session)==NIMBY_UNSUPPORTED_GAME);
        assert(session==0);
    }
    assert(GetProcessHandleCount(GetCurrentProcess(),&after));
    assert(before==after);
}
