#pragma once
/**
 * @file rate_matching.hpp
 * @brief Simplified rate matching (puncturing/repetition).
 *
 * TRACEABILITY NOTE: Conceptually related to TS 38.212 §5.4.2 rate matching for
 * LDPC. This educational path uses bit selection/repetition only — NOT full
 * bit-interleaved circular buffer RM conformance.
 */
#include "nr_bb/types.hpp"
#include <cstddef>

namespace nr_bb {

BitVec rate_match(const BitVec& coded, std::size_t e_bits);
SoftVec rate_dematch(const SoftVec& llr_e, std::size_t n_coded);

}  // namespace nr_bb
