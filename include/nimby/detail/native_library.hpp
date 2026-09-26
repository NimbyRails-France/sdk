#pragma once
// Native module ownership contract (identical on both platforms):
// load() and existing() acquire a reference; each reference needs unload().
// symbol() borrows an address valid only while its module remains loaded.
// pin=true retains executable code until process exit (Kotlin GC threads).
// Module loading and unloading must happen outside the Windows loader lock.
#if defined(_WIN32)
#include "platform/windows/native_library.hpp"
#elif defined(__linux__)
#include "platform/linux/native_library.hpp"
#else
#error Unsupported SDK platform
#endif
