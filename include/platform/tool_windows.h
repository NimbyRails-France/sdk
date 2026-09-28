#pragma once
// Backend selector only. Tool forms are currently shipped on Windows.
#if defined(_WIN32)
#include <platform/windows/mod/tool_windows.h>
namespace nimby::platform { using ToolWindows=windows::ToolWindows; }
#else
#error Tool windows are not implemented for this platform
#endif
