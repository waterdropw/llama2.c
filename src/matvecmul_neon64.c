//
// Created by xbwee on 2023/8/25.
//

#include "matvecmul_row.h"
#include <arm_neon.h>

#ifdef __cplusplus
namespace hpc {
extern "C" {
#endif

#if defined(HAS_MATVECMUL_NEON64)
void MatVecMulRow_NEON64(const float* W, int stride, const float* x, float* y, int cols) {
  __asm__ volatile (
      // 遍历一行，每次处理4个元素，累加结果暂存到寄存器 v0 中
      "movi   v0.2d, #0000000000000000             \n"
      "add    w8, w4, #4            \n"
      "1:                           \n"
      "ldr     q1, [x0], #16        \n"
      "ldr     q2, [x2], #16        \n"
      "sub     w8, w8, #4           \n"
      "cmp     w8, #4               \n"
      "fmla    v0.4s, v2.4s, v1.4s  \n"
      "b.gt    1b                   \n"
      "faddp   v0.4s, v0.4s, v0.4s  \n"
      "faddp   s0, v0.2s            \n"
      "str     s0, [x3]             \n"
      : "+r"(W),        // %0
        "+r"(stride),   // %1
        "+r"(x),        // %2
        "+r"(y)         // %3
      : "r"(cols)       // %4
      : "cc", "memory", "v0", "v1", "v2"  // Clobber List，no need list the general registers (the compiler will handle it)
      );
}
#endif  // defined(HAS_MATVECMUL_NEON64)

#ifdef __cplusplus
}  // extern "C"
}  // namespace hpc
#endif
