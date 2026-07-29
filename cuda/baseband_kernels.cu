// CUDA baseband candidate kernels — modulate / demod / scramble / rate-match.
// When no GPU is present, scripts MUST emit BLOCKED_HARDWARE (never invent timings).
// CPU reference: nr_bb::cuda_ref::*
#include <cstdint>

namespace nr_bb {
namespace cuda_bb {

__global__ void scramble_kernel(const uint8_t* in, const uint8_t* gold, uint8_t* out, int n) {
  int i = blockIdx.x * blockDim.x + threadIdx.x;
  if (i < n) out[i] = in[i] ^ gold[i];
}

__global__ void rate_match_kernel(const uint8_t* coded, uint8_t* out, int n_coded, int e_bits) {
  int i = blockIdx.x * blockDim.x + threadIdx.x;
  if (i < e_bits) out[i] = coded[i % n_coded];
}

__global__ void qpsk_mod_kernel(const uint8_t* bits, float* iq, int n_bits) {
  int s = blockIdx.x * blockDim.x + threadIdx.x;
  int n_sym = n_bits / 2;
  if (s < n_sym) {
    /* TS 38.211-style QPSK: (1-2b)/√2 */
    float re = (bits[2 * s] == 0) ? 0.70710678f : -0.70710678f;
    float im = (bits[2 * s + 1] == 0) ? 0.70710678f : -0.70710678f;
    iq[2 * s] = re;
    iq[2 * s + 1] = im;
  }
}

__global__ void qpsk_llr_kernel(const float* iq, float* llr, float inv_noise, int n_sym) {
  int s = blockIdx.x * blockDim.x + threadIdx.x;
  if (s < n_sym) {
    llr[2 * s] = 2.0f * inv_noise * iq[2 * s];
    llr[2 * s + 1] = 2.0f * inv_noise * iq[2 * s + 1];
  }
}

void launch_scramble(const uint8_t* in, const uint8_t* gold, uint8_t* out, int n) {
  int threads = 256;
  int blocks = (n + threads - 1) / threads;
  scramble_kernel<<<blocks, threads>>>(in, gold, out, n);
}

void launch_rate_match(const uint8_t* coded, uint8_t* out, int n_coded, int e_bits) {
  int threads = 256;
  int blocks = (e_bits + threads - 1) / threads;
  rate_match_kernel<<<blocks, threads>>>(coded, out, n_coded, e_bits);
}

void launch_qpsk_mod(const uint8_t* bits, float* iq, int n_bits) {
  int n_sym = n_bits / 2;
  int threads = 256;
  int blocks = (n_sym + threads - 1) / threads;
  qpsk_mod_kernel<<<blocks, threads>>>(bits, iq, n_bits);
}

void launch_qpsk_llr(const float* iq, float* llr, float noise_var, int n_sym) {
  int threads = 256;
  int blocks = (n_sym + threads - 1) / threads;
  qpsk_llr_kernel<<<blocks, threads>>>(iq, llr, 1.0f / noise_var, n_sym);
}

}  // namespace cuda_bb
}  // namespace nr_bb
