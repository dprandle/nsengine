#ifndef NSTYPES_H
#define NSTYPES_H

#include <stdint.h>
 
typedef wchar_t wchar;

#ifdef WIN32
typedef wchar_t char16;
typedef wchar_t char32;
#else
typedef char16_t char16;
typedef char32_t char32;
#endif

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef long double ldouble;

#endif
