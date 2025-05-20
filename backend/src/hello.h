#pragma once

#include <windows.h>

#ifdef DLL_EXPORTS
#define DLL_API __declspec(dllexport) 
#else
#define DLL_API __declspec(dllimport) 
#endif

#ifdef  __cplusplus
extern "C" {
#endif

	DLL_API int WINAPI hello();

#ifdef  __cplusplus
}
#endif