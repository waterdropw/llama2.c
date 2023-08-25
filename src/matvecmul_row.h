//
// Created by weixiaobin on 2023/8/25.
//

#ifndef LLAMA_C_MATVECMUL_ROW_H
#define LLAMA_C_MATVECMUL_ROW_H

#include "basic_types.h"

#ifdef __cplusplus
namespace hpc {
extern "C" {
#endif

// NOTES(xbwee): We DON'T care about the platforms, just care about the
// compiler features. Will check the hw features at runtime later.

// The following are available on all x86 platforms:
#if !defined(DISABLE_X86) && \
    (defined(_M_IX86) || defined(__x86_64__) || defined(__i386__))

#endif

// The following are available on all x86 platforms, but
// require VS2012, clang 3.4 or gcc 4.7.
#if !defined(DISABLE_X86) &&                          \
    (defined(VISUALC_HAS_AVX2) || defined(CLANG_HAS_AVX2) || \
     defined(GCC_HAS_AVX2))
#define HAS_MATVECMUL_AVX2
#endif

// The following are available for AVX512 clang x86 platforms:
#if !defined(DISABLE_X86) && \
    (defined(__x86_64__) || defined(__i386__)) && defined(CLANG_HAS_AVX512)
#define HAS_MATVECMUL_AVX512
#endif
// The following are available for AVX512 clang x64 platforms only:
// TODO(xbwee): Port to x86
#if !defined(DISABLE_X86) && defined(__x86_64__) && \
    (defined(CLANG_HAS_AVX512))

#endif

// The following are available on Neon platforms:
#if !defined(DISABLE_NEON) && (defined(__aarch64__) || defined(__ARM_NEON__))
#define HAS_MATVECMUL_NEON
#endif
// The following are available on AArch64 platforms only:
// TODO(xbwee): Port to AArch32
#if !defined(DISABLE_NEON) && defined(__aarch64__)
#define HAS_MATVECMUL_NEON64
#endif


void MatVecMulRow_NAIVE(const float* W, int stride, const float* x, float* y, int cols);
//void MatVecMulCx8_NAIVE(const float* W, int stride, const float* x, float* y, int cols);
//void MatVecMulCx16_NAIVE(const float* W, int stride, const float* x, float* y, int cols);

void MatVecMulRow_NEON(const float* W, int stride, const float* x, float* y, int cols);
void MatVecMulRow_NEON64(const float* W, int stride, const float* x, float* y, int cols);
void MatVecMulRow_AVX2(const float* W, int stride, const float* x, float* y, int cols);
void MatVecMulRow_AVX512(const float* W, int stride, const float* x, float* y, int cols);


#ifdef __cplusplus
}  // extern "C"
}  // namespace hpc
#endif
#endif // LLAMA_C_MATVECMUL_ROW_H
