#pragma once
// Private backend selector. No operating-system handles enter the adapter API.
#if defined(_WIN32)
#include "windows/train_length.hpp"
#elif defined(__linux__)
#include "linux/train_length.hpp"
#else
#error Unsupported SDK platform
#endif
