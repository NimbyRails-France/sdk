#include "platform/observation_process.h"
#include "engine/binary_identity.h"
#include "platform/linux/process.h"
#include <unistd.h>
#include <array>
#include <cstring>

namespace nimby::platform {
struct ObservationProcess::Impl { std::unique_ptr<linux_os::Process> process; };
ObservationProcess::ObservationProcess() : impl_(std::make_unique<Impl>()) {}
ObservationProcess::~ObservationProcess() = default;
engine::LiveStateProfile ObservationProcess::profile() noexcept {
    return engine::LiveStateProfile::Linux119;
}
bool ObservationProcess::alive() const noexcept { return impl_->process && impl_->process->alive(); }
bool ObservationProcess::read(uint64_t address,void* output,size_t size) const noexcept {
    return impl_->process && impl_->process->read(address,output,size);
}
uint32_t ObservationProcess::open(uint32_t requestedPid) {
    // A connection is created once per session; the registry owns cleanup even
    // if an intermediate OS call fails. Metadata is never published on failure.
        pid=requestedPid?requestedPid:static_cast<uint32_t>(getpid());
        impl_->process=std::make_unique<nimby::platform::linux_os::Process>(pid);
        const auto path=impl_->process->executable();
        game_directory=path.parent_path();
        const auto identity=nimby::platform::linux_os::identify(path);
        if(identity.size!=20787376 || identity.sha256!="2581d0e8157f43acb137b2bd9d52e2a7c82bd8af8b62fab5d87f00cc27eefde6")return NIMBY_UNSUPPORTED_GAME;
        binary.struct_size=sizeof binary;binary.recognized_research_build=1;binary.file_size=identity.size;
        std::memcpy(binary.sha256,identity.sha256.c_str(),65);
        base=impl_->process->image_base();
        if(!alive())return NIMBY_PROCESS_EXITED;
        std::array<unsigned char,4> header{};
        if(!impl_->process->read(base,header.data(),header.size()))return NIMBY_IO_ERROR;
    return NIMBY_OK;
}

uint32_t ObservationProcess::setClock(int64_t utc_seconds,NimbySimulationClock& output) noexcept {
    (void)utc_seconds; (void)output;
    return NIMBY_CLOCK_WRITE_FAILED;
}
uint32_t ObservationProcess::setClockAndRecalculate(int64_t utc,NimbySimulationClock& output,uint32_t& countValue) noexcept {
    (void)utc; (void)output; (void)countValue;
    return NIMBY_CLOCK_WRITE_FAILED;
}
uint32_t ObservationProcess::construction(const NimbyConstructionRequest*,uint64_t,NimbyConstructionResult&) noexcept {
    return NIMBY_HOOKS_UNAVAILABLE;
}
}
