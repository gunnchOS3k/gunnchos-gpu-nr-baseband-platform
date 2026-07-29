#pragma once
/**
 * @file modulation.hpp
 * @brief Exact Gray-mapped QPSK/16QAM/64QAM/256QAM + soft max-log-MAP LLR.
 *
 * TRACEABILITY NOTE: Constellation construction follows TS 38.211 §5.1.3 Gray
 * tables conceptually (unit-energy). Soft demod uses max-log-MAP over the QAM
 * alphabet. NOT a vendor modulator bit-exact claim.
 */
#include "nr_bb/types.hpp"
#include <vector>

namespace nr_bb {

ComplexVec modulate(const BitVec& bits, ModulationOrder order);
BitVec demodulate_hard(const ComplexVec& syms, ModulationOrder order);

/**
 * Soft LLR L(b) = log P(b=0|y)/P(b=1|y) via max-log:
 * L_k ≈ (1/(2σ²)) * (min_{s:bk=1} |y-s|² − min_{s:bk=0} |y-s|²).
 */
SoftVec demodulate_llr(const ComplexVec& syms, ModulationOrder order, double noise_var);

int bits_per_symbol(ModulationOrder order);

/** Unit-energy constellation points in Gray bit order 0..(M-1). */
std::vector<Complex> constellation(ModulationOrder order);

/** Average symbol energy of constellation (should be ≈1). */
double constellation_avg_energy(ModulationOrder order);

}  // namespace nr_bb
