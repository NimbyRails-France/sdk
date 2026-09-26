#pragma once
/* ABI spelling only. No runtime platform behavior belongs in this selector. */
#if defined(_WIN32)
#include "windows/export.h"
#elif defined(__linux__)
#include "linux/export.h"
#else
#error Unsupported SDK platform
#endif
