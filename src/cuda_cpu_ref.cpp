#include "nr_bb/cuda_cpu_ref.hpp"
#include "nr_bb/modulation.hpp"
#include "nr_bb/rate_matching.hpp"
#include "nr_bb/scrambling.hpp"
#include <cmath>

namespace nr_bb {
namespace cuda_ref {

BitVec scramble_bits(const BitVec& bits, uint32_t c_init) { return scramble(bits, c_init); }

BitVec rate_match_bits(const BitVec& coded, std::size_t e_bits) { return rate_match(coded, e_bits); }

std::vector<float> modulate_qpsk_f32(const BitVec& bits) {
  auto sym = modulate(bits, ModulationOrder::QPSK);
  std::vector<float> out(sym.size() * 2);
  for (std::size_t i = 0; i < sym.size(); ++i) {
    out[2 * i] = static_cast<float>(sym[i].real());
    out[2 * i + 1] = static_cast<float>(sym[i].imag());
  }
  return out;
}

std::vector<float> qpsk_llr_f32(const std::vector<float>& iq, float noise_var) {
  require(iq.size() % 2 == 0, "iq interleaved length");
  require(noise_var > 0, "noise_var");
  std::vector<float> llr(iq.size());  // 2 bits per symbol
  const float scale = 2.0f / noise_var;  // max-log for QPSK on unit energy: L=2*y/σ²
  for (std::size_t i = 0; i < iq.size(); i += 2) {
    llr[i] = scale * iq[i];          // bit0 from I
    llr[i + 1] = scale * iq[i + 1];  // bit1 from Q
  }
  return llr;
}

CompareReport compare_f32(const std::vector<float>& a, const std::vector<float>& b, const std::string& kernel,
                          double tol) {
  CompareReport r;
  r.kernel = kernel;
  if (a.size() != b.size()) {
    r.mismatches = std::max(a.size(), b.size());
    r.status = "length_mismatch";
    return r;
  }
  for (std::size_t i = 0; i < a.size(); ++i) {
    const double e = std::abs(static_cast<double>(a[i]) - static_cast<double>(b[i]));
    if (e > r.max_abs_err) r.max_abs_err = e;
    if (e > tol) ++r.mismatches;
  }
  r.match = r.mismatches == 0;
  r.status = r.match ? "cpu_reference_match" : "cpu_reference_mismatch";
  return r;
}

}  // namespace cuda_ref
}  // namespace nr_bb
