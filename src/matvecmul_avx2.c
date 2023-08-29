//
// Created by xbwee on 2023/8/29.
//
#include <stdio.h>
#include "matvecmul_row.h"

#if defined(HAS_MATVECMUL_AVX2)
#include <immintrin.h>

#ifdef __cplusplus
namespace hpc {
extern "C" {
#endif

void MatVecMulRow_AVX2(const float* w, int stride, const float* x, float* y, int cols) {
  __m256 sum_vec = _mm256_setzero_ps(); // for AVX2, sum of 8 floats
  float *wptr = (float *)w;
  float *xptr = (float *)x;
  do {
    __m256 w_vec = _mm256_loadu_ps(wptr); // w is not aligned
    __m256 x_vec = _mm256_load_ps(xptr);
    __m256 prod_vec = _mm256_mul_ps(w_vec, x_vec);
    sum_vec = _mm256_add_ps(sum_vec, prod_vec);

    wptr += 8;
    xptr += 8;
    cols -= 8;
  } while (cols > 0);
  // Perform horizontal add
  sum_vec = _mm256_hadd_ps(sum_vec, sum_vec);
  sum_vec = _mm256_hadd_ps(sum_vec, sum_vec);
  float vals[8];
  _mm256_storeu_ps(vals, sum_vec);
  *y = vals[0] + vals[4];
}

#ifdef __cplusplus
}  // extern "C"
}  // namespace hpc
#endif
#endif  // defined(HAS_MATVECMUL_AVX2)