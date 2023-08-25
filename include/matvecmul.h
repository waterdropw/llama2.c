//
// Created by xbwee on 2023/8/25.
//

#ifndef LLAMA_C_MATVECMUL_H
#define LLAMA_C_MATVECMUL_H

#ifdef __cplusplus
namespace hpc {
extern "C" {
#endif

// Matrix multiply vector, Y=Wx for MxN matrix W and Nx1 vector X
void MatVecMul(const float* W, const float* x, float* y, int M, int N);


#ifdef __cplusplus
}  // extern "C"
}  // namespace hpc
#endif
#endif // LLAMA_C_MATVECMUL_H
