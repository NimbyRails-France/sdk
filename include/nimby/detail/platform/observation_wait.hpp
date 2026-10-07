#pragma once
#if defined(_WIN32)
#include "windows/observation_wait.hpp"
#elif defined(__linux__)
#include "linux/observation_wait.hpp"
#else
#error Unsupported SDK platform
#endif
