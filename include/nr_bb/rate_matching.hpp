#pragma once
/**
 * @file rate_matching.hpp
 * @brief NR LDPC rate matching / recovery (circular buffer + RV k0).
 *
 * TRACEABILITY: TS 38.212 §5.4.2 — bit selection with RV start k0 from
 * Table 5.4.2.1-2 coefficients. Circular buffer excludes the first 2Zc
 * systematically punctured bits.
 */
#include "nr_bb/ldpc.hpp"
#include "nr_bb/types.hpp"
#include <cstddef>
#include <vector>

namespace nr_bb {

struct RateMatchConfig {
  std::size_t e_bits = 0;  // output length E
  int rv = 0;              // redundancy version 0..3
  std::size_t n_cb = 0;    // circular buffer size (0 => derive)
  int zc = 0;              // lifting size (for k0 / puncture of 2Z)
  BaseGraph bg = BaseGraph::BG2;
  bool puncture_first_2z = true;
};

/** k0 start positions for RV0..RV3 (spec buffer space). */
std::vector<std::size_t> rv_k0_positions(std::size_t n_cb, int zc, BaseGraph bg);

BitVec rate_match(const BitVec& coded, std::size_t e_bits);
BitVec rate_match(const BitVec& coded, const RateMatchConfig& cfg);

SoftVec rate_dematch(const SoftVec& llr_e, std::size_t n_coded);
SoftVec rate_dematch(const SoftVec& llr_e, const RateMatchConfig& cfg, std::size_t n_coded);

}  // namespace nr_bb
