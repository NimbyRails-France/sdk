#pragma once
#include <nimby/detail/sdk.h>
#include <nimby/detail/native_library.hpp>
#include <cstdint>
namespace nimby::detail::platform {
struct Discovery { uint32_t status; uint32_t pid; };
}
// This file only selects a backend. OS calls belong in windows/ or linux/.
#if defined(_WIN32)
#include "windows/host.hpp"
#elif defined(__linux__)
#include "linux/host.hpp"
#else
#error Unsupported SDK platform
#endif
