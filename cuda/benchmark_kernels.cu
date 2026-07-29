// CUDA graph/stream benchmark candidate kernels.
// When no GPU is present, profiling scripts MUST label BLOCKED_HARDWARE.
#include <cstdint>

namespace nr_bb {
namespace cuda_bench {

__global__ void axpy_kernel(const float* x, float* y, float a, int n) {
  int i = blockIdx.x * blockDim.x + threadIdx.x;
  if (i < n) y[i] = a * x[i] + y[i];
}

void launch_axpy(const float* x, float* y, float a, int n) {
  int threads = 256;
  int blocks = (n + threads - 1) / threads;
  axpy_kernel<<<blocks, threads>>>(x, y, a, n);
}

}  // namespace cuda_bench
}  // namespace nr_bb
