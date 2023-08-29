//
// Created by xbwee on 2023/8/25.
//

#include "matvecmul_row.h"

#ifdef __cplusplus
namespace hpc {
extern "C" {
#endif

void MatVecMulRow_NAIVE(const float* w, int stride, const float* x, float* y, int cols) {
  float val = 0.0f;
  for (int j = 0; j < cols; j++) {
    val += w[j] * x[j];
  }
  y[0] = val;
}

#ifdef __cplusplus
}  // extern "C"
}  // namespace hpc
#endif