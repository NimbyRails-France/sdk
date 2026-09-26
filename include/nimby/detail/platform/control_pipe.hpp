#pragma once
#if defined(_WIN32)
#include "windows/control_pipe.hpp"
#else
#include "linux/control_pipe.hpp"
#endif
