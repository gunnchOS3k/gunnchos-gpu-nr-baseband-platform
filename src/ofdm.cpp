#include "nr_bb/ofdm.hpp"
#include <cmath>
#include <numbers>

namespace nr_bb {

namespace {
ComplexVec dft(const ComplexVec& in, bool inverse) {
  const std::size_t N = in.size();
  ComplexVec out(N);
  const double sign = inverse ? 1.0 : -1.0;
  const double scale = inverse ? (1.0 / static_cast<double>(N)) : 1.0;
  for (std::size_t k = 0; k < N; ++k) {
    Complex sum{0, 0};
    for (std::size_t n = 0; n < N; ++n) {
      const double ang = sign * 2.0 * std::numbers::pi * static_cast<double>(k) * static_cast<double>(n) /
                         static_cast<double>(N);
      sum += in[n] * Complex{std::cos(ang), std::sin(ang)};
    }
    out[k] = sum * scale;
  }
  return out;
}
}  // namespace

ComplexVec ofdm_modulate(const ComplexVec& freq_symbols, const OfdmConfig& cfg) {
  require(freq_symbols.size() == cfg.fft_size, "freq_symbols size must equal fft_size");
  auto time = dft(freq_symbols, true);
  ComplexVec out;
  out.reserve(cfg.cp_len + cfg.fft_size);
  out.insert(out.end(), time.end() - static_cast<std::ptrdiff_t>(cfg.cp_len), time.end());
  out.insert(out.end(), time.begin(), time.end());
  return out;
}

ComplexVec ofdm_demodulate(const ComplexVec& time_signal, const OfdmConfig& cfg) {
  require(time_signal.size() == cfg.cp_len + cfg.fft_size, "time signal length mismatch");
  ComplexVec useful(time_signal.begin() + static_cast<std::ptrdiff_t>(cfg.cp_len), time_signal.end());
  return dft(useful, false);
}

}  // namespace nr_bb
