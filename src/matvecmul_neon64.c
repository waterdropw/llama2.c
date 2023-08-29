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
void MatVecMulRow_NEON64(const float* w, int stride, const float* x, float* y, int cols) {
//  printf("NEON64\n");
#if 0
  __asm__ volatile (
      // 遍历一行，每次处理4个元素，累加结果暂存到寄存器 v0 中
      "movi   v0.4s, #0             \n"
      "add    x8, x4, #4            \n"
      "1:                           \n"
      "ld1     {v1.4s}, [%0], #16        \n"
      "ld1     {v2.4s}, [%2], #16        \n"
      "sub     w8, w8, #4           \n"
      "cmp     w8, #4               \n"
      "fmla    v0.4s, v2.4s, v1.4s  \n"
      "b.gt    1b                   \n"
      "faddp   v0.4s, v0.4s, v0.4s  \n"
      "faddp   s0, v0.2s            \n"
      "str     s0, [%3]             \n"
      : "+r"(w),        // %0
        "+r"(stride),   // %1
        "+r"(x),        // %2
        "+r"(y)         // %3
      : "r"(cols)       // %4
      : "cc", "memory", "v0", "v1", "v2"  // Clobber List，no need list the general registers (the compiler will handle it)
      );
#else
  __asm__ volatile (
      // 遍历一行，每次处理8个元素，累加结果暂存到寄存器 v0,v1 中
      "movi   v0.2d, #0000000000000000             \n"
      "add    w8, w4, #8            \n"
      "movi   v1.2d, #0000000000000000             \n"
      "1:                           \n"
      "ldp     q2, q3, [%0], #32    \n"   // ldx/ldr/ldp 偏移按字节单位计数
      "subs    w8, w8, #8           \n"
      "ldp     q4, q5, [%2], #32    \n"
      "fmla    v0.4s, v2.4s, v4.4s  \n"
      "fmla    v0.4s, v3.4s, v5.4s  \n"
      "b.ne    1b                   \n"
      "fadd    v0.4s, v0.4s, v1.4s  \n"
      "faddp   v0.4s, v0.4s, v0.4s  \n"
      "faddp   s0, v0.2s            \n"
      "str     s0, [%3]             \n"
      : "+r"(w),        // %0
        "+r"(stride),   // %1
        "+r"(x),        // %2
        "+r"(y)         // %3
      : "r"(cols)       // %4
      : "cc", "memory", "v0", "v1", "v2", "v3", "v4", "v5"  // Clobber List，no need list the general registers (the compiler will handle it)
  );
#endif
}
#endif  // defined(HAS_MATVECMUL_NEON64)

#ifdef __cplusplus
}  // extern "C"
}  // namespace hpc
#endif
