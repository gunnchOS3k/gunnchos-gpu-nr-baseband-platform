// Extended CUDA baseband / PHY candidate kernels.
// CPU references: nr_bb::cuda_ref::*
// Without GPU: scripts MUST emit BLOCKED_HARDWARE (never invent timings).
// cuFFT OFDM is a lab candidate — declared via documentation in cuda/README.md;
// this translation unit avoids a hard cufft link dependency so CPU trees stay clean.
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

__global__ void zf_tone_kernel(const float* y_re, const float* y_im, const float* h_re, const float* h_im,
                               float* x_re, float* x_im, int n) {
  int i = blockIdx.x * blockDim.x + threadIdx.x;
  if (i < n) {
    float den = h_re[i] * h_re[i] + h_im[i] * h_im[i] + 1e-12f;
    x_re[i] = (y_re[i] * h_re[i] + y_im[i] * h_im[i]) / den;
    x_im[i] = (y_im[i] * h_re[i] - y_re[i] * h_im[i]) / den;
  }
}

__global__ void mmse_tone_kernel(const float* y_re, const float* y_im, const float* h_re, const float* h_im,
                                 float noise, float* x_re, float* x_im, int n) {
  int i = blockIdx.x * blockDim.x + threadIdx.x;
  if (i < n) {
    float den = h_re[i] * h_re[i] + h_im[i] * h_im[i] + noise;
    x_re[i] = (y_re[i] * h_re[i] + y_im[i] * h_im[i]) / den;
    x_im[i] = (y_im[i] * h_re[i] - y_re[i] * h_im[i]) / den;
  }
}

__global__ void harq_combine_kernel(const float* a, const float* b, float* out, int n) {
  int i = blockIdx.x * blockDim.x + threadIdx.x;
  if (i < n) out[i] = a[i] + b[i];
}

__global__ void ls_channel_est_kernel(const float* y_re, const float* y_im, const float* p_re, const float* p_im,
                                      float* h_re, float* h_im, int n) {
  int i = blockIdx.x * blockDim.x + threadIdx.x;
  if (i < n) {
    float den = p_re[i] * p_re[i] + p_im[i] * p_im[i] + 1e-12f;
    h_re[i] = (y_re[i] * p_re[i] + y_im[i] * p_im[i]) / den;
    h_im[i] = (y_im[i] * p_re[i] - y_re[i] * p_im[i]) / den;
  }
}

__global__ void scheduler_metric_kernel(const float* se, const float* weight, float* metric, int n) {
  int i = blockIdx.x * blockDim.x + threadIdx.x;
  if (i < n) metric[i] = se[i] * weight[i];
}

void launch_scramble(const uint8_t* in, const uint8_t* gold, uint8_t* out, int n) {
  int threads = 256;
  scramble_kernel<<<(n + threads - 1) / threads, threads>>>(in, gold, out, n);
}

void launch_rate_match(const uint8_t* coded, uint8_t* out, int n_coded, int e_bits) {
  int threads = 256;
  rate_match_kernel<<<(e_bits + threads - 1) / threads, threads>>>(coded, out, n_coded, e_bits);
}

void launch_qpsk_mod(const uint8_t* bits, float* iq, int n_bits) {
  int n_sym = n_bits / 2;
  int threads = 256;
  qpsk_mod_kernel<<<(n_sym + threads - 1) / threads, threads>>>(bits, iq, n_bits);
}

void launch_qpsk_llr(const float* iq, float* llr, float noise_var, int n_sym) {
  int threads = 256;
  qpsk_llr_kernel<<<(n_sym + threads - 1) / threads, threads>>>(iq, llr, 1.0f / noise_var, n_sym);
}

void launch_zf(const float* y_re, const float* y_im, const float* h_re, const float* h_im, float* x_re,
               float* x_im, int n) {
  int threads = 256;
  zf_tone_kernel<<<(n + threads - 1) / threads, threads>>>(y_re, y_im, h_re, h_im, x_re, x_im, n);
}

void launch_mmse(const float* y_re, const float* y_im, const float* h_re, const float* h_im, float noise,
                 float* x_re, float* x_im, int n) {
  int threads = 256;
  mmse_tone_kernel<<<(n + threads - 1) / threads, threads>>>(y_re, y_im, h_re, h_im, noise, x_re, x_im, n);
}

void launch_harq_combine(const float* a, const float* b, float* out, int n) {
  int threads = 256;
  harq_combine_kernel<<<(n + threads - 1) / threads, threads>>>(a, b, out, n);
}

void launch_ls_est(const float* y_re, const float* y_im, const float* p_re, const float* p_im, float* h_re,
                   float* h_im, int n) {
  int threads = 256;
  ls_channel_est_kernel<<<(n + threads - 1) / threads, threads>>>(y_re, y_im, p_re, p_im, h_re, h_im, n);
}

void launch_scheduler_metric(const float* se, const float* weight, float* metric, int n) {
  int threads = 256;
  scheduler_metric_kernel<<<(n + threads - 1) / threads, threads>>>(se, weight, metric, n);
}

}  // namespace cuda_bb
}  // namespace nr_bb
