#pragma once
#if defined(_WIN32)
#include "windows/paths.hpp"
#elif defined(__linux__)
#include "linux/paths.hpp"
#else
#error Unsupported SDK platform
#endif
