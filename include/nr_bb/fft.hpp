#pragma once
/**
 * @file fft.hpp
 * @brief Radix-2 FFT/IFFT used on the acceptance OFDM path.
 *
 * TRACEABILITY NOTE: Numerically replaces educational O(N²) DFT. Algorithm is
 * Cooley–Tukey radix-2 (public domain). NOT a claim of vendor FFT library parity.
 */
#include "nr_bb/types.hpp"
#include <cstddef>

namespace nr_bb {

/** In-place-capable out-of-place radix-2 FFT. N must be power of two. */
ComplexVec fft_radix2(const ComplexVec& in, bool inverse);

bool is_power_of_two(std::size_t n);

}  // namespace nr_bb
