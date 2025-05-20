

#pragma once

#include "compiler.h"
#include "headers.h"

#ifdef XCORE_API_USE_DYNAMIC_LINKING
#  if XCORE_COMPILER == XCORE_COMPILER_MICROSOFT
#    define X_API_EXPORT __declspec(dllexport)
#    define X_API_IMPORT __declspec(dllimport)
#  elif XCORE_COMPILER == XCORE_COMPILER_GNU
#    define X_API_EXPORT __attribute__((visibility("default")))
#    define X_API_IMPORT
#  else
#    error compiler not supported!
#  endif
#else
#  define X_API_EXPORT
#  define X_API_IMPORT
#endif

#ifdef XCORE_API_EXPORT
#  define XCORE_API X_API_EXPORT
#else
#  define XCORE_API X_API_IMPORT
#endif



#define UI64FMTD "%" PRIu64
#define UI64LIT(N) UINT64_C(N)

#define SI64FMTD "%" PRId64
#define SI64LIT(N) INT64_C(N)

#define SZFMTD "%" PRIuPTR

#define STRING_VIEW_FMT "%.*s"
#define STRING_VIEW_FMT_ARG(str) static_cast<int>((str).length()), (str).data()

typedef int64_t int64;
typedef int32_t int32;
typedef int16_t int16;
typedef int8_t int8;
typedef uint64_t uint64;
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef uint8_t uint8;

using byte=uint8;

/// undefine min max
#define NOMINMAX
