//
// Created by xbwee on 2023/8/25.
//
#include <stdio.h>
#include "cpuinfo.h"
#include "matvecmul.h"
#include "matvecmul_row.h"

#ifdef __cplusplus
namespace hpc {
extern "C" {
#endif

// Processor memory alignment stride
#define CACHE_ALIGN_SIZE 32

static void MatVecMul_L1D32KB_L2D256KB(const float* W, const float* x, float* y, int M, int N);

void MatVecMul(const float* W, const float* x, float* y, int M, int N) {
  MatVecMul_L1D32KB_L2D256KB(W, x, y, M, N);
}

void* aligned_calloc(size_t nitems, size_t size) {
  void* mem = NULL;
  size_t block_size = nitems * size;
  if (posix_memalign(&mem, CACHE_ALIGN_SIZE, block_size)) {
    return NULL;
  }
  return mem;
}

// 32*1024/(768*4) = 10.67, 256*1024/(768*4) = 85.33
// L1D 可存储10行，L2D 可存储85行，因此循环展开每次计算8行(取2^n倍数)，可以保证L1D命中率；
// 再对W矩阵分块，每块不超过84行，可以保证L2D命中率，这里取64(2^n的倍数)。
static void MatVecMul_L1D32KB_L2D256KB(const float* W, const float* x, float* y, int M, int N) {
  void (*MatVecMulRow)(const float* w_row, int stride, const float* x, float* y, int cols) = MatVecMulRow_NAIVE;
//  printf("W: %p, x: %p, y: %p, M: %d, N: %d\n", W, x, y, M, N);
//  printf("W is aligned: %d, x is aligned: %d, y is aligned: %d\n", IS_ALIGNED(W, 8), IS_ALIGNED(x, 8), IS_ALIGNED(y, 8));
//  printf("has neon: %d, has neon64: %d, has avx2: %d\n", cpuinfo_has_arm_neon(), cpuinfo_has_arm_neon_v8(), cpuinfo_has_x86_avx2());

#if defined(HAS_MATVECMUL_NEON)
  if (/*IS_ALIGNED(W, 8) && */
      IS_ALIGNED(x, 8) && IS_ALIGNED(y, 8) && cpuinfo_has_arm_neon()) {
    MatVecMulRow = MatVecMulRow_NEON;
  }
#endif
#if defined(HAS_MATVECMUL_NEON64)
  if (/*IS_ALIGNED(W, 8) && */
      IS_ALIGNED(x, 8) && IS_ALIGNED(y, 8) && cpuinfo_has_arm_neon_v8()) {
    MatVecMulRow = MatVecMulRow_NEON64;
  }
#endif
#if defined(HAS_MATVECMUL_AVX2)
  if (/*IS_ALIGNED(W, 8) && */
      IS_ALIGNED(x, 8) && IS_ALIGNED(y, 8) && cpuinfo_has_x86_avx2()) {
    MatVecMulRow = MatVecMulRow_AVX2;
  }
#endif

#pragma omp parallel for thread_number(4)
  for (int block = 0; block < M; block += 64) {
    for (int i = 0; i < 64; i+=8) {
      int index = block + i;
      MatVecMulRow(W+(index+0)*N, N, x, &y[index+0], N);
      MatVecMulRow(W+(index+1)*N, N, x, &y[index+1], N);
      MatVecMulRow(W+(index+2)*N, N, x, &y[index+2], N);
      MatVecMulRow(W+(index+3)*N, N, x, &y[index+3], N);
      MatVecMulRow(W+(index+4)*N, N, x, &y[index+4], N);
      MatVecMulRow(W+(index+5)*N, N, x, &y[index+5], N);
      MatVecMulRow(W+(index+6)*N, N, x, &y[index+6], N);
      MatVecMulRow(W+(index+7)*N, N, x, &y[index+7], N);
    }
  }
}

#ifdef __cplusplus
}  // extern "C"
}  // namespace hpc
#endif