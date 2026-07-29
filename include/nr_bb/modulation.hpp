#pragma once
/**
 * @file modulation.hpp
 * @brief QPSK / 16QAM / 64QAM / 256QAM modulate & hard demodulate.
 *
 * TRACEABILITY NOTE: Constellations follow Gray-mapped NR tables conceptually
 * from TS 38.211 §5.1. Educational unit-energy mapping; NOT a claim of full
 * 3GPP modulator conformance (scaling/offsets may differ).
 */
#include "nr_bb/types.hpp"

namespace nr_bb {

ComplexVec modulate(const BitVec& bits, ModulationOrder order);
BitVec demodulate_hard(const ComplexVec& syms, ModulationOrder order);
SoftVec demodulate_llr(const ComplexVec& syms, ModulationOrder order, double noise_var);

int bits_per_symbol(ModulationOrder order);

}  // namespace nr_bb
