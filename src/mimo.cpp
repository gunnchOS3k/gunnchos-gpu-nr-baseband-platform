#include "nr_bb/mimo.hpp"

namespace nr_bb {

MimoChannelConfig identity_mimo(int n, double snr_db, uint32_t seed) {
  require(n == 1 || n == 2 || n == 4, "n must be 1,2,4");
  MimoChannelConfig cfg;
  cfg.n_tx = n;
  cfg.n_rx = n;
  cfg.snr_db = snr_db;
  cfg.rng_seed = seed;
  cfg.H.assign(static_cast<size_t>(n * n), Complex{0, 0});
  for (int i = 0; i < n; ++i) cfg.H[static_cast<size_t>(i * n + i)] = Complex{1, 0};
  return cfg;
}

MimoLinkResult run_mimo_link(const std::vector<ComplexVec>& tx, const MimoChannelConfig& ch,
                             const EqualizerConfig& eq) {
  MimoLinkResult r;
  r.rx = apply_mimo_channel(tx, ch);
  r.equalized = equalize_stream(r.rx, ch.H, ch.n_tx, eq);
  return r;
}

}  // namespace nr_bb
