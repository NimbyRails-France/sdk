#pragma once
#include <nimby/detail/platform/windows/system.hpp>
// Loader-lock invariant: no allocation, locks, threads, I/O or callbacks here.
// NRFMod_StartV1 / NRFMod_StopV1 own explicit lifecycle outside DllMain.
BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID) { return TRUE; }
