#include "nr_bb/channel.hpp"
#include <cmath>
#include <numbers>
#include <random>

namespace nr_bb {

namespace {
double awgn_sigma(double snr_db) {
  const double snr = std::pow(10.0, snr_db / 10.0);
  return 1.0 / std::sqrt(2.0 * snr);  // per dimension for unit-power
}
}  // namespace

ComplexVec apply_siso_channel(const ComplexVec& tx, const ChannelConfig& cfg) {
  std::mt19937 rng(cfg.rng_seed);
  std::normal_distribution<double> gauss(0.0, awgn_sigma(cfg.snr_db));
  ComplexVec delayed;
  if (cfg.timing_offset_samples >= 0) {
    delayed.assign(static_cast<size_t>(cfg.timing_offset_samples), Complex{0, 0});
    delayed.insert(delayed.end(), tx.begin(), tx.end());
  } else {
    const std::size_t drop = static_cast<std::size_t>(-cfg.timing_offset_samples);
    require(drop < tx.size(), "timing offset drops entire signal");
    delayed.assign(tx.begin() + static_cast<std::ptrdiff_t>(drop), tx.end());
  }
  ComplexVec rx(delayed.size());
  for (std::size_t n = 0; n < delayed.size(); ++n) {
    const double t = static_cast<double>(n) / cfg.sample_rate_hz;
    const double ang = 2.0 * std::numbers::pi * cfg.cfo_hz * t;
    const Complex rot{std::cos(ang), std::sin(ang)};
    rx[n] = delayed[n] * rot + Complex{gauss(rng), gauss(rng)};
  }
  return rx;
}

std::vector<ComplexVec> apply_mimo_channel(const std::vector<ComplexVec>& tx_layers,
                                           const MimoChannelConfig& cfg) {
  require(static_cast<int>(tx_layers.size()) == cfg.n_tx, "tx layer count mismatch");
  require(static_cast<int>(cfg.H.size()) == cfg.n_rx * cfg.n_tx, "H size mismatch");
  require(!tx_layers.empty(), "empty tx");
  const std::size_t T = tx_layers[0].size();
  for (const auto& L : tx_layers) require(L.size() == T, "tx length mismatch");

  std::mt19937 rng(cfg.rng_seed);
  std::normal_distribution<double> gauss(0.0, awgn_sigma(cfg.snr_db));
  std::vector<ComplexVec> rx(static_cast<size_t>(cfg.n_rx), ComplexVec(T));
  for (std::size_t t = 0; t < T; ++t) {
    for (int r = 0; r < cfg.n_rx; ++r) {
      Complex sum{0, 0};
      for (int c = 0; c < cfg.n_tx; ++c) {
        sum += cfg.H[static_cast<size_t>(r * cfg.n_tx + c)] * tx_layers[static_cast<size_t>(c)][t];
      }
      const double ang = 2.0 * std::numbers::pi * cfg.cfo_hz * static_cast<double>(t) / cfg.sample_rate_hz;
      sum *= Complex{std::cos(ang), std::sin(ang)};
      sum += Complex{gauss(rng), gauss(rng)};
      rx[static_cast<size_t>(r)][t] = sum;
    }
  }
  return rx;
}

}  // namespace nr_bb
