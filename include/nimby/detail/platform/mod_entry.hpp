#pragma once
#if defined(_WIN32)
#include "windows/mod_entry.hpp"
#elif defined(__linux__)
#include "linux/mod_entry.hpp"
#else
#error Unsupported SDK platform
#endif
