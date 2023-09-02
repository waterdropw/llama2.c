//
// Created by xbwee on 2023/8/25.
//

#include "matvecmul_row.h"

#if defined(HAS_MATVECMUL_NEON)
#include <arm_neon.h>

#ifdef __cplusplus
namespace hpc {
extern "C" {
#endif

void MatVecMulRow_NEON(const float* w, int stride, const float* x, float* y, int cols) {
//  printf("NEON\n");
  float32x4_t vec4_w;
  float32x4_t vec4_x;
  float32x4_t vec4_sum = vdupq_n_f32(0.0f);
  do {
    vec4_w = vld1q_f32(w);
    vec4_x = vld1q_f32(x);
    vec4_sum = vfmaq_f32(vec4_sum, vec4_w, vec4_x);
    w += 4;
    x += 4;
    cols -= 4;
  } while (cols > 0);
  *y = vaddvq_f32(vec4_sum);
}

// process a cache line (64bytes = 16 x float) per loop.
void MatVecMulRow_NEON_16(const float* w, int stride, const float* x, float* y, int cols) {
  //  printf("NEON\n");
  float32x4_t vec4_w0, vec4_w1, vec4_w2, vec4_w3;
  float32x4_t vec4_x0, vec4_x1, vec4_x2, vec4_x3;
  float32x4_t vec4_sum0 = vdupq_n_f32(0.0f);
  float32x4_t vec4_sum1 = vdupq_n_f32(0.0f);
  float32x4_t vec4_sum2 = vdupq_n_f32(0.0f);
  float32x4_t vec4_sum3 = vdupq_n_f32(0.0f);
  do {
    vec4_w0 = vld1q_f32(w);
    vec4_w0 = vld1q_f32(w + 4);
    vec4_w0 = vld1q_f32(w + 8);
    vec4_w0 = vld1q_f32(w + 12);
    vec4_x0 = vld1q_f32(x);
    vec4_x1 = vld1q_f32(x + 4);
    vec4_x2 = vld1q_f32(x + 8);
    vec4_x3 = vld1q_f32(x + 12);

    vec4_sum0 = vfmaq_f32(vec4_sum0, vec4_w0, vec4_x0);
    vec4_sum1 = vfmaq_f32(vec4_sum1, vec4_w1, vec4_x1);
    vec4_sum2 = vfmaq_f32(vec4_sum2, vec4_w2, vec4_x2);
    vec4_sum3 = vfmaq_f32(vec4_sum3, vec4_w3, vec4_x3);
    w += 16;
    x += 16;
    cols -= 16;
  } while (cols > 0);
  float32x4_t result_sum = vaddq_f32(vaddq_f32(vec4_sum0, vec4_sum1), vaddq_f32(vec4_sum2, vec4_sum3));
  *y = vaddvq_f32(result_sum);
}


#ifdef __cplusplus
}  // extern "C"
}  // namespace hpc
#endif

#endif  // defined(HAS_MATVECMUL_NEON)