#pragma once
/**
 * @file channel_estimation.hpp
 * @brief LS channel estimation from DMRS.
 *
 * TRACEABILITY NOTE: Educational LS estimator. Not a claim of NR DMRS-based
 * channel estimation conformance (no interpolation/filtering model claim).
 */
#include "nr_bb/types.hpp"
#include <vector>

namespace nr_bb {

/** LS estimate per subcarrier: Y / X for DMRS tones. */
ComplexVec ls_channel_estimate(const ComplexVec& rx_dmrs, const ComplexVec& tx_dmrs);

/** Expand per-SC estimate to a time-frequency grid by hold. */
std::vector<ComplexVec> hold_interpolate(const ComplexVec& h_sc, int n_symb);

}  // namespace nr_bb
