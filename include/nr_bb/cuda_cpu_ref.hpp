#pragma once
/**
 * @file cuda_cpu_ref.hpp
 * @brief CPU reference implementations mirroring CUDA baseband kernels.
 *
 * Comparison schema: bit-exact / L∞ for float kernels. GPU numeric runs on Mac
 * are BLOCKED_HARDWARE.
 */
#include "nr_bb/types.hpp"
#include <string>
#include <vector>

namespace nr_bb {
namespace cuda_ref {

struct CompareReport {
  bool match = false;
  double max_abs_err = 0;
  std::size_t mismatches = 0;
  std::string kernel;
  std::string status = "cpu_reference_only";
};

BitVec scramble_bits(const BitVec& bits, uint32_t c_init);
BitVec rate_match_bits(const BitVec& coded, std::size_t e_bits);
/** Soft demod QPSK LLRs (float path for CUDA parity). */
std::vector<float> qpsk_llr_f32(const std::vector<float>& iq_interleaved /* Re,Im,... */, float noise_var);
std::vector<float> modulate_qpsk_f32(const BitVec& bits);

CompareReport compare_f32(const std::vector<float>& a, const std::vector<float>& b, const std::string& kernel,
                          double tol = 1e-5);

}  // namespace cuda_ref
}  // namespace nr_bb
