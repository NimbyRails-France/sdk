#pragma once
#ifdef _WIN32
#include <nimby/detail/platform/windows/diagnostics.hpp>
#else
// Linux support is paused. Keep the public/common boundary compilable without
// introducing a Windows dependency or a new Linux runtime implementation.
namespace nimby::detail::diagnostics {
inline void write(const char*, const char*, const char*) noexcept {}
inline void exception(const char*, const char*) noexcept {}
}
#endif
