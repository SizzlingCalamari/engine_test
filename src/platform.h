
#ifdef PLATFORM_PCH
#  error this is a pch, don't include it...
#else
#  define PLATFORM_PCH
#endif

#include <cstdint>

// STDCALL define
#ifndef STDCALL
#  ifdef _WIN32
#    if defined(__MINGW32__) || defined(__CYGWIN__)
#      define STDCALL __stdcall
#    else (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED) || defined(__BORLANDC__)
#      define STDCALL __stdcall
#    endif
#  endif // _WIN32
#  ifndef STDCALL
#    define STDCALL
#  endif // STDCALL
#endif // STDCALL

// types
typedef unsigned int uint;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
