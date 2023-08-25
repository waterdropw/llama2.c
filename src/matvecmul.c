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
  void (*MatVecMulRow)(const float* W, int stride, const float* x, float* y, int cols) = MatVecMulRow_NAIVE;
#if defined(HAS_MATVECMUL_NEON)
  if (IS_ALIGNED(W, 8) && IS_ALIGNED(x, 8) && cpuinfo_has_arm_neon()) {
    MatVecMulRow = MatVecMulRow_NEON;
  }
#endif
#if defined(HAS_MATVECMUL_NEON64)
  if (IS_ALIGNED(W, 8) && IS_ALIGNED(x, 8) && cpuinfo_has_arm_neon_v8()) {
    MatVecMulRow = MatVecMulRow_NEON64;
  }
#endif
#if defined(HAS_MATVECMUL_AVX2)
  if (IS_ALIGNED(W, 8) && IS_ALIGNED(x, 8) && cpuinfo_has_x86_avx2()) {
    MatVecMulRow = MatVecMulRow_AVX2;
  }
#endif

  for (int i = 0; i < M; i++) {
    MatVecMulRow(W, N, x, y, N);
    W += N;
    y++;
  }
}

#ifdef __cplusplus
}  // extern "C"
}  // namespace hpc
#endif