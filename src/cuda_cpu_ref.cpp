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
  std::vector<float> llr(iq.size());
  const float scale = 2.0f / noise_var;
  for (std::size_t i = 0; i < iq.size(); i += 2) {
    llr[i] = scale * iq[i];
    llr[i + 1] = scale * iq[i + 1];
  }
  return llr;
}

void zf_tone_f32(const std::vector<float>& y_re, const std::vector<float>& y_im, const std::vector<float>& h_re,
                 const std::vector<float>& h_im, std::vector<float>& x_re, std::vector<float>& x_im) {
  require(y_re.size() == y_im.size() && y_re.size() == h_re.size() && h_re.size() == h_im.size(), "zf sizes");
  x_re.resize(y_re.size());
  x_im.resize(y_re.size());
  for (std::size_t i = 0; i < y_re.size(); ++i) {
    const float den = h_re[i] * h_re[i] + h_im[i] * h_im[i] + 1e-12f;
    x_re[i] = (y_re[i] * h_re[i] + y_im[i] * h_im[i]) / den;
    x_im[i] = (y_im[i] * h_re[i] - y_re[i] * h_im[i]) / den;
  }
}

void mmse_tone_f32(const std::vector<float>& y_re, const std::vector<float>& y_im, const std::vector<float>& h_re,
                   const std::vector<float>& h_im, float noise, std::vector<float>& x_re, std::vector<float>& x_im) {
  require(noise > 0, "noise");
  x_re.resize(y_re.size());
  x_im.resize(y_re.size());
  for (std::size_t i = 0; i < y_re.size(); ++i) {
    const float den = h_re[i] * h_re[i] + h_im[i] * h_im[i] + noise;
    x_re[i] = (y_re[i] * h_re[i] + y_im[i] * h_im[i]) / den;
    x_im[i] = (y_im[i] * h_re[i] - y_re[i] * h_im[i]) / den;
  }
}

std::vector<float> harq_combine_f32(const std::vector<float>& a, const std::vector<float>& b) {
  require(a.size() == b.size(), "harq size");
  std::vector<float> out(a.size());
  for (std::size_t i = 0; i < a.size(); ++i) out[i] = a[i] + b[i];
  return out;
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
