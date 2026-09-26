#pragma once
#if defined(_WIN32)
#include "windows/mod_abi.hpp"
#elif defined(__linux__)
#include "linux/mod_abi.hpp"
#else
#error Unsupported SDK platform
#endif
