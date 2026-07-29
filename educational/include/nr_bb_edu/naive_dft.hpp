#pragma once
/** EDUCATIONAL_ONLY — O(N²) DFT. Acceptance OFDM uses radix-2 FFT. */
#include "nr_bb/types.hpp"

namespace nr_bb_edu {

nr_bb::ComplexVec naive_dft(const nr_bb::ComplexVec& in, bool inverse);

}  // namespace nr_bb_edu
