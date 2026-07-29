#pragma once
/**
 * @file dmrs.hpp
 * @brief Simplified PUSCH DMRS sequence generation and placement.
 *
 * TRACEABILITY NOTE: Conceptually TS 38.211 §6.4.1.1 PUSCH DMRS.
 * Uses Gold-derived QPSK reference symbols on a regular comb — educational,
 * NOT full DMRS configuration / OCC / port mapping conformance.
 */
#include "nr_bb/types.hpp"
#include <cstdint>
#include <vector>

namespace nr_bb {

struct DmrsConfig {
  int n_sc = 12;           // subcarriers in allocation
  int n_symb = 14;         // OFDM symbols in slot
  int dmrs_symb = 2;       // symbol index for DMRS
  uint32_t c_init = 1;
};

struct DmrsGrid {
  std::vector<ComplexVec> grid;  // [symb][sc], 0 where no DMRS
  ComplexVec sequence;
};

DmrsGrid generate_dmrs(const DmrsConfig& cfg);

}  // namespace nr_bb
