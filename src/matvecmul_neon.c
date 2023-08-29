//
// Created by xbwee on 2023/8/25.
//

#include "matvecmul_row.h"
#include <arm_neon.h>

#ifdef __cplusplus
namespace hpc {
extern "C" {
#endif

#if defined(HAS_MATVECMUL_NEON)
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
#endif  // defined(HAS_MATVECMUL_INTRINSICS)

#ifdef __cplusplus
}  // extern "C"
}  // namespace hpc
#endif
