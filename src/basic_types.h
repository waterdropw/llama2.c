//
// Created by xbwee on 2023/8/25.
//

#ifndef LLAMA_C_BASIC_TYPES_H
#define LLAMA_C_BASIC_TYPES_H

#include <stddef.h>  // For size_t and NULL
#include <stdlib.h>  // For malloc

#if !defined(INT_TYPES_DEFINED) && !defined(GG_LONGLONG)
#define INT_TYPES_DEFINED

#if defined(_MSC_VER) && (_MSC_VER < 1600)
#include <sys/types.h>  // for uintptr_t on x86
typedef unsigned __int64 uint64_t;
typedef __int64 int64_t;
typedef unsigned int uint32_t;
typedef int int32_t;
typedef unsigned short uint16_t;
typedef short int16_t;
typedef unsigned char uint8_t;
typedef signed char int8_t;
#else
#include <stdint.h>  // for uintptr_t and C99 types
#endif               // defined(_MSC_VER) && (_MSC_VER < 1600)

#endif  // INT_TYPES_DEFINED

#if !defined(API)
#if defined(_WIN32) || defined(__CYGWIN__)
#if defined(BUILDING_SHARED_LIBRARY)
#define API __declspec(dllexport)
#elif defined(USING_SHARED_LIBRARY)
#define API __declspec(dllimport)
#else
#define API
#endif  // BUILDING_SHARED_LIBRARY
#elif defined(__GNUC__) && (__GNUC__ >= 4) && !defined(__APPLE__) && \
    (defined(BUILDING_SHARED_LIBRARY) ||                      \
     defined(USING_SHARED_LIBRARY))
#define API __attribute__((visibility("default")))
#else
#define API
#endif  // __GNUC__
#endif  // API

#ifdef __cplusplus
namespace hpc {
extern "C" {
#endif

#if defined(__pnacl__) || defined(__CLR_VER) ||            \
    (defined(__native_client__) && defined(__x86_64__)) || \
    (defined(__i386__) && !defined(__SSE__) && !defined(__clang__))
#define DISABLE_X86
#endif
#if defined(__native_client__)
#define DISABLE_NEON
#endif
// MemorySanitizer does not support assembly code yet. http://crbug.com/344505
#if defined(__has_feature)
#if __has_feature(memory_sanitizer)
#define DISABLE_X86
#endif
#endif
// clang >= 3.5.0 required for Arm64.
#if defined(__clang__) && defined(__aarch64__) && !defined(DISABLE_NEON)
#if (__clang_major__ < 3) || (__clang_major__ == 3 && (__clang_minor__ < 5))
#define DISABLE_NEON
#endif  // clang >= 3.5
#endif  // __clang__

// GCC >= 4.7.0 required for AVX2.
#if defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#if (__GNUC__ > 4) || (__GNUC__ == 4 && (__GNUC_MINOR__ >= 7))
#define GCC_HAS_AVX2 1
#endif  // GNUC >= 4.7
#endif  // __GNUC__

// clang >= 3.4.0 required for AVX2.
#if defined(__clang__) && (defined(__x86_64__) || defined(__i386__))
#if (__clang_major__ > 3) || (__clang_major__ == 3 && (__clang_minor__ >= 4))
#define CLANG_HAS_AVX2 1
#endif  // clang >= 3.4
#endif  // __clang__

// clang >= 6.0.0 required for AVX512.
#if defined(__clang__) && (defined(__x86_64__) || defined(__i386__))
// clang in xcode follows a different versioning scheme.
// TODO(fbarchard): fix xcode 9 ios b/789.
#if (__clang_major__ >= 7) && !defined(__APPLE__)
#define CLANG_HAS_AVX512 1
#endif  // clang >= 7
#endif  // __clang__

// Visual C 2012 required for AVX2.
#if defined(_M_IX86) && !defined(__clang__) && defined(_MSC_VER) && \
    _MSC_VER >= 1700
#define VISUALC_HAS_AVX2 1
#endif  // VisualStudio >= 2012

#if defined(_MSC_VER) && !defined(__CLR_VER) && !defined(__clang__)
#if defined(VISUALC_HAS_AVX2)
#define SIMD_ALIGNED(var) __declspec(align(32)) var
#else
#define SIMD_ALIGNED(var) __declspec(align(16)) var
#endif
#define NOINLINE __declspec(noinline)
typedef __declspec(align(16)) int16_t vec16[8];
typedef __declspec(align(16)) int32_t vec32[4];
typedef __declspec(align(16)) float vecf32[4];
typedef __declspec(align(16)) int8_t vec8[16];
typedef __declspec(align(16)) uint16_t uvec16[8];
typedef __declspec(align(16)) uint32_t uvec32[4];
typedef __declspec(align(16)) uint8_t uvec8[16];
typedef __declspec(align(32)) int16_t lvec16[16];
typedef __declspec(align(32)) int32_t lvec32[8];
typedef __declspec(align(32)) int8_t lvec8[32];
typedef __declspec(align(32)) uint16_t ulvec16[16];
typedef __declspec(align(32)) uint32_t ulvec32[8];
typedef __declspec(align(32)) uint8_t ulvec8[32];
#elif !defined(__pnacl__) && (defined(__GNUC__) || defined(__clang__))
// Caveat GCC 4.2 to 4.7 have a known issue using vectors with const.
#if defined(CLANG_HAS_AVX2) || defined(GCC_HAS_AVX2)
#define SIMD_ALIGNED(var) var __attribute__((aligned(32)))
#else
#define SIMD_ALIGNED(var) var __attribute__((aligned(16)))
#endif
#define IS_ALIGNED(p, n) (!((uintptr_t)(p) & ((n)-1)))
#define NOINLINE __attribute__((noinline))

typedef int16_t __attribute__((vector_size(16))) vec16;
typedef int32_t __attribute__((vector_size(16))) vec32;
typedef float __attribute__((vector_size(16))) vecf32;
typedef int8_t __attribute__((vector_size(16))) vec8;
typedef uint16_t __attribute__((vector_size(16))) uvec16;
typedef uint32_t __attribute__((vector_size(16))) uvec32;
typedef uint8_t __attribute__((vector_size(16))) uvec8;
typedef int16_t __attribute__((vector_size(32))) lvec16;
typedef int32_t __attribute__((vector_size(32))) lvec32;
typedef int8_t __attribute__((vector_size(32))) lvec8;
typedef uint16_t __attribute__((vector_size(32))) ulvec16;
typedef uint32_t __attribute__((vector_size(32))) ulvec32;
typedef uint8_t __attribute__((vector_size(32))) ulvec8;
#else
#define SIMD_ALIGNED(var) var
#define NOINLINE
typedef int16_t vec16[8];
typedef int32_t vec32[4];
typedef float vecf32[4];
typedef int8_t vec8[16];
typedef uint16_t uvec16[8];
typedef uint32_t uvec32[4];
typedef uint8_t uvec8[16];
typedef int16_t lvec16[16];
typedef int32_t lvec32[8];
typedef int8_t lvec8[32];
typedef uint16_t ulvec16[16];
typedef uint32_t ulvec32[8];
typedef uint8_t ulvec8[32];
#endif

#ifdef __cplusplus
}  // extern "C"
}  // namespace hpc
#endif

#endif // LLAMA_C_BASIC_TYPES_H
