#pragma once
/**
 * @file rate_matching.hpp
 * @brief Rate matching / recovery scaffolding (puncture, repeat, circular buffer).
 *
 * TRACEABILITY NOTE: Inspired by TS 38.212 §5.4.2 bit selection. Implements
 * circular-buffer style E-bit selection and LLR recovery combination. NOT a
 * claim of full NR bit-interleaver / RV table conformance.
 */
#include "nr_bb/types.hpp"
#include <cstddef>

namespace nr_bb {

struct RateMatchConfig {
  std::size_t e_bits = 0;   // output length E
  int rv = 0;               // redundancy version index 0..3 (start offset hint)
  std::size_t n_cb = 0;     // circular buffer size (0 => coded.size())
};

BitVec rate_match(const BitVec& coded, std::size_t e_bits);
BitVec rate_match(const BitVec& coded, const RateMatchConfig& cfg);

SoftVec rate_dematch(const SoftVec& llr_e, std::size_t n_coded);
SoftVec rate_dematch(const SoftVec& llr_e, const RateMatchConfig& cfg, std::size_t n_coded);

}  // namespace nr_bb
