#include "nr_bb/ofdm.hpp"
#include "nr_bb/fft.hpp"

namespace nr_bb {

ComplexVec ofdm_modulate(const ComplexVec& freq_symbols, const OfdmConfig& cfg) {
  require(freq_symbols.size() == cfg.fft_size, "freq_symbols size must equal fft_size");
  require(is_power_of_two(cfg.fft_size), "fft_size must be power of two");
  auto time = fft_radix2(freq_symbols, /*inverse=*/true);
  ComplexVec out;
  out.reserve(cfg.cp_len + cfg.fft_size);
  out.insert(out.end(), time.end() - static_cast<std::ptrdiff_t>(cfg.cp_len), time.end());
  out.insert(out.end(), time.begin(), time.end());
  return out;
}

ComplexVec ofdm_demodulate(const ComplexVec& time_signal, const OfdmConfig& cfg) {
  require(time_signal.size() == cfg.cp_len + cfg.fft_size, "time signal length mismatch");
  require(is_power_of_two(cfg.fft_size), "fft_size must be power of two");
  ComplexVec useful(time_signal.begin() + static_cast<std::ptrdiff_t>(cfg.cp_len), time_signal.end());
  return fft_radix2(useful, /*inverse=*/false);
}

}  // namespace nr_bb
