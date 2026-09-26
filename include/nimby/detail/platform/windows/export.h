#pragma once
#if defined(NIMBY_SDK_BUILD)
#define NIMBY_API __declspec(dllexport)
#else
#define NIMBY_API __declspec(dllimport)
#endif
