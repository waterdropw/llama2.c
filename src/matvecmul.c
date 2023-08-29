//
// Created by xbwee on 2023/8/25.
//

#include "matvecmul.h"
#include "matvecmul_row.h"
#include "cpuinfo.h"

#ifdef __cplusplus
namespace hpc {
extern "C" {
#endif

void MatVecMul(const float* W, const float* x, float* y, int M, int N) {
  void (*MatVecMulRow)(const float* w_row, int stride, const float* x, float* y, int cols) = MatVecMulRow_NAIVE;
//  printf("W: %p, x: %p, y: %p, M: %d, N: %d\n", W, x, y, M, N);
//  printf("W is aligned: %d, x is aligned: %d, y is aligned: %d\n", IS_ALIGNED(W, 8), IS_ALIGNED(x, 8), IS_ALIGNED(y, 8));

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
      IS_ALIGNED(x, 8) && cpuinfo_has_x86_avx2()) {
    MatVecMulRow = MatVecMulRow_AVX2;
  }
#endif

#pragma omp parallel for thread_number(4)
  for (int i = 0; i <= M - 8; i+=8) {
    MatVecMulRow(W+(i+0)*N, N, x, &y[i+0], N);
    MatVecMulRow(W+(i+1)*N, N, x, &y[i+1], N);
    MatVecMulRow(W+(i+2)*N, N, x, &y[i+2], N);
    MatVecMulRow(W+(i+3)*N, N, x, &y[i+3], N);
    MatVecMulRow(W+(i+4)*N, N, x, &y[i+4], N);
    MatVecMulRow(W+(i+5)*N, N, x, &y[i+5], N);
    MatVecMulRow(W+(i+6)*N, N, x, &y[i+6], N);
    MatVecMulRow(W+(i+7)*N, N, x, &y[i+7], N);
  }
}

#ifdef __cplusplus
}  // extern "C"
}  // namespace hpc
#endif