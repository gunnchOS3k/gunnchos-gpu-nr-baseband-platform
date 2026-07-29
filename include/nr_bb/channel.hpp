#pragma once
/**
 * @file channel.hpp
 * @brief AWGN / flat MIMO channel with CFO and timing offset models.
 *
 * TRACEABILITY NOTE: Impairment models are research/educational. Not a
 * 3GPP channel model (TDL/CDL) conformance claim.
 */
#include "nr_bb/types.hpp"
#include <cstdint>
#include <vector>

namespace nr_bb {

struct ChannelConfig {
  double snr_db = 20.0;
  double cfo_hz = 0.0;
  double sample_rate_hz = 1e6;
  int timing_offset_samples = 0;
  uint32_t rng_seed = 1;
};

/** Apply flat scalar channel + AWGN + CFO + integer timing offset (pad/drop). */
ComplexVec apply_siso_channel(const ComplexVec& tx, const ChannelConfig& cfg);

/** MIMO flat channel: H is n_rx * n_tx row-major complex gains per sample time. */
struct MimoChannelConfig : ChannelConfig {
  int n_tx = 1;
  int n_rx = 1;
  std::vector<Complex> H;  // size n_rx*n_tx
};

/** tx_layers[tx][t] -> rx_layers[rx][t] */
std::vector<ComplexVec> apply_mimo_channel(const std::vector<ComplexVec>& tx_layers,
                                           const MimoChannelConfig& cfg);

}  // namespace nr_bb
