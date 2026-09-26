#include <windows.h>
// Windows loader lock: never start threads, perform I/O or acquire SDK locks.
// Runtime initialization and shutdown are explicit exported operations.
BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID) { return TRUE; }
