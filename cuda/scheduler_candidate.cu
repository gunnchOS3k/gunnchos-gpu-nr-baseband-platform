// CUDA PF scheduler candidate kernels (research).
// TRACEABILITY NOTE: NOT 3GPP MAC conformance.
// On hosts without NVIDIA GPU these sources are not built; gate scripts
// must emit BLOCKED_HARDWARE rather than fabricating results.
#include <cstdint>

namespace nr_bb {
namespace cuda_sched {

__global__ void pf_metric_kernel(const float* instant, const float* avg, float* out, int n) {
  int i = blockIdx.x * blockDim.x + threadIdx.x;
  if (i < n) {
    float d = avg[i];
    if (d < 1e-9f) d = 1e-9f;
    out[i] = instant[i] / d;
  }
}

void launch_pf_metric(const float* instant, const float* avg, float* out, int n) {
  int threads = 256;
  int blocks = (n + threads - 1) / threads;
  pf_metric_kernel<<<blocks, threads>>>(instant, avg, out, n);
}

}  // namespace cuda_sched
}  // namespace nr_bb
