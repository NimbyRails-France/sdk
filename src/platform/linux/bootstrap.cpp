#include "platform/linux/process.h"
#include <nimby/detail/sdk.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <unistd.h>

extern "C" NIMBY_API uint32_t NimbyInternal_StartLinuxObservationBroker() noexcept {
    try {
        static std::mutex mutex;std::lock_guard guard(mutex);
        static std::unique_ptr<nimby::platform::linux_os::MemoryBroker> broker;
        if(broker)return NIMBY_ALREADY_INITIALIZED;
        const auto identity=nimby::platform::linux_os::identify("/proc/self/exe");
        if(identity.size!=20787376||identity.sha256!="2581d0e8157f43acb137b2bd9d52e2a7c82bd8af8b62fab5d87f00cc27eefde6")return NIMBY_INVALID_BINARY;
        broker=std::make_unique<nimby::platform::linux_os::MemoryBroker>();
        std::fprintf(stderr,"[NRF SDK] Linux observation broker ready, PID %d\n",getpid());
        return NIMBY_OK;
    }catch(...){return NIMBY_IO_ERROR;}
}
__attribute__((constructor)) static void startRequestedBroker() {
    const auto* enabled=std::getenv("NRF_LINUX_OBSERVATION");
    if(enabled&&std::strcmp(enabled,"1")==0) {
        const auto status=NimbyInternal_StartLinuxObservationBroker();
        if(status!=NIMBY_OK&&status!=NIMBY_ALREADY_INITIALIZED)
            std::fprintf(stderr,"[NRF SDK] Linux observation broker unavailable: %u\n",status);
    }
}
