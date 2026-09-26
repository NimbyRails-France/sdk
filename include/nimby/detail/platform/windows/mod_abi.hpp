#pragma once
#include <nimby/detail/platform/windows/system.hpp>
#define NRF_CALL WINAPI
#define NRF_MOD_EXPORT extern "C" __declspec(dllexport) uint32_t NRF_CALL
