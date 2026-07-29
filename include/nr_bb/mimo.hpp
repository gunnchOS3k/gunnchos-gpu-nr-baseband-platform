#pragma once
/**
 * @file mimo.hpp
 * @brief SISO / 2x2 / 4x4 MIMO helpers wrapping channel + equalizer.
 *
 * TRACEABILITY NOTE: Research MIMO link helpers. Not NR codebook conformance.
 */
#include "nr_bb/channel.hpp"
#include "nr_bb/equalizer.hpp"

namespace nr_bb {

struct MimoLinkResult {
  std::vector<ComplexVec> rx;
  std::vector<ComplexVec> equalized;
};

MimoChannelConfig identity_mimo(int n, double snr_db, uint32_t seed = 1);
MimoLinkResult run_mimo_link(const std::vector<ComplexVec>& tx, const MimoChannelConfig& ch,
                             const EqualizerConfig& eq);

}  // namespace nr_bb
