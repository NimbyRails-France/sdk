#pragma once
#include <nimby/detail/mod_host.h>
#include <nimby/detail/platform/host.hpp>
namespace nimby::detail {
// Internal SDK orchestration only; mods do not need to supply diagnostic
// stages. This updates a fixed shared marker, without formatting or file I/O.
inline void markModWork(uint32_t stage,uint64_t detail=0) noexcept {
    platform::modWorkStage(stage,detail);
}
}
