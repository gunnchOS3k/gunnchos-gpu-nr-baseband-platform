#pragma once
/**
 * @file ofdm.hpp
 * @brief Educational OFDM modulate/demodulate (IFFT/FFT with CP).
 *
 * TRACEABILITY NOTE: Conceptually TS 38.211 §5.3 OFDM baseband signal
 * generation. Uses naive DFT; NOT a claim of NR numerology/CP conformance.
 */
#include "nr_bb/types.hpp"
#include <cstddef>

namespace nr_bb {

struct OfdmConfig {
  std::size_t fft_size = 64;
  std::size_t cp_len = 8;
};

ComplexVec ofdm_modulate(const ComplexVec& freq_symbols, const OfdmConfig& cfg);
ComplexVec ofdm_demodulate(const ComplexVec& time_signal, const OfdmConfig& cfg);

}  // namespace nr_bb
