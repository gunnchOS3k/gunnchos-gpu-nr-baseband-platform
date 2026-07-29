#pragma once
/**
 * @file equalizer.hpp
 * @brief ZF / MMSE / regularized ZF MIMO equalization.
 *
 * TRACEABILITY NOTE: Classical linear equalizers for research benchmarking.
 * Not a 3GPP-specified algorithm claim.
 */
#include "nr_bb/types.hpp"
#include <vector>

namespace nr_bb {

struct EqualizerConfig {
  EqualizerType type = EqualizerType::MMSE;
  double noise_var = 0.01;
  double rzf_delta = 0.01;  // regularization
};

/**
 * Equalize one time sample: y (n_rx), H (n_rx x n_tx row-major) -> x_hat (n_tx).
 */
ComplexVec equalize_sample(const ComplexVec& y, const std::vector<Complex>& H, int n_rx, int n_tx,
                           const EqualizerConfig& cfg);

std::vector<ComplexVec> equalize_stream(const std::vector<ComplexVec>& y_rx,
                                        const std::vector<Complex>& H, int n_tx,
                                        const EqualizerConfig& cfg);

}  // namespace nr_bb
