#pragma once

#define XCORE_PLATFORM_WINDOWS 0
#define XCORE_PLATFORM_UNIX    1
#define XCORE_PLATFORM_APPLE   2
#define XCORE_PLATFORM_IOS     3



#ifdef _WIN32 
#  define XCORE_PLATFORM XCORE_PLATFORM_WINDOWS
#elif  __APPLE__ 
    #include "TargetConditionals.h"
    #if TARGET_IPHONE_SIMULATOR
    #elif TARGET_OS_IPHONE
    #  define XCORE_PLATFORM XCORE_PLATFORM_IOS
    #elif TARGET_OS_MAC
    #  define XCORE_PLATFORM XCORE_PLATFORM_APPLE
    #else
    #   error "Unknow Apple platform"
    #endif
#elif __linux__ || __unix__
#  define XCORE_PLATFORM XCORE_PLATFORM_UNIX
#endif

#define XCORE_COMPILER_MICROSOFT 0
#define XCORE_COMPILER_GNU       1
#define XCORE_COMPILER_INTEL     2

#ifdef _MSC_VER
#  define XCORE_COMPILER XCORE_COMPILER_MICROSOFT
#elif defined( __INTEL_COMPILER )
#  define XCORE_COMPILER XCORE_COMPILER_INTEL
#elif defined( __GNUC__ )
#  define XCORE_COMPILER XCORE_COMPILER_GNU
#  define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#else
#  error "FATAL ERROR: Unknown compiler."
#endif

