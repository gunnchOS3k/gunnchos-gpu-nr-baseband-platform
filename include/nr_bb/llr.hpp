#pragma once
/**
 * @file llr.hpp
 * @brief LLR helpers and bit metrics.
 *
 * TRACEABILITY NOTE: Soft-bit interfaces used after equalization / demodulation.
 * Educational; not a claim of NR soft-demodulator conformance.
 */
#include "nr_bb/types.hpp"

namespace nr_bb {

/** Convert hard bits to LLRs with given magnitude (bit0 => +mag). */
SoftVec bits_to_llr(const BitVec& bits, double magnitude = 1.0);

/** Hard decisions from LLRs (LLR>=0 => 0). */
BitVec llr_to_bits(const SoftVec& llr);

/** Combine retransmission LLRs (Chase combining). */
SoftVec llr_combine(const SoftVec& a, const SoftVec& b);

}  // namespace nr_bb
