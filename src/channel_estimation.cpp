#include "nr_bb/channel_estimation.hpp"
#include <cmath>

namespace nr_bb {

ComplexVec ls_channel_estimate(const ComplexVec& rx_dmrs, const ComplexVec& tx_dmrs) {
  require(rx_dmrs.size() == tx_dmrs.size(), "DMRS size mismatch");
  ComplexVec H(rx_dmrs.size());
  for (std::size_t i = 0; i < rx_dmrs.size(); ++i) {
    const double p = std::norm(tx_dmrs[i]);
    require(p > 1e-12, "zero DMRS pilot");
    H[i] = rx_dmrs[i] / tx_dmrs[i];
  }
  return H;
}

std::vector<ComplexVec> hold_interpolate(const ComplexVec& h_sc, int n_symb) {
  require(n_symb > 0, "n_symb must be > 0");
  return std::vector<ComplexVec>(static_cast<size_t>(n_symb), h_sc);
}

ComplexVec interpolate_frequency(const ComplexVec& h_pilots, const std::vector<int>& pilot_idx,
                                 int n_sc) {
  require(h_pilots.size() == pilot_idx.size(), "pilot size mismatch");
  require(!pilot_idx.empty(), "no pilots");
  require(n_sc > 0, "n_sc");
  ComplexVec H(static_cast<size_t>(n_sc), {0, 0});
  for (int sc = 0; sc < n_sc; ++sc) {
    // find neighboring pilots
    int lo = 0, hi = static_cast<int>(pilot_idx.size()) - 1;
    if (sc <= pilot_idx.front()) {
      H[static_cast<size_t>(sc)] = h_pilots.front();
      continue;
    }
    if (sc >= pilot_idx.back()) {
      H[static_cast<size_t>(sc)] = h_pilots.back();
      continue;
    }
    while (hi - lo > 1) {
      const int mid = (lo + hi) / 2;
      if (pilot_idx[static_cast<size_t>(mid)] <= sc) lo = mid;
      else hi = mid;
    }
    const double x0 = pilot_idx[static_cast<size_t>(lo)];
    const double x1 = pilot_idx[static_cast<size_t>(hi)];
    const double t = (static_cast<double>(sc) - x0) / std::max(1e-12, x1 - x0);
    H[static_cast<size_t>(sc)] = h_pilots[static_cast<size_t>(lo)] * (1.0 - t) +
                                 h_pilots[static_cast<size_t>(hi)] * t;
  }
  return H;
}

ComplexVec equalize_zf_tones(const ComplexVec& y, const ComplexVec& H) {
  require(y.size() == H.size(), "ZF size mismatch");
  ComplexVec x(y.size());
  for (std::size_t i = 0; i < y.size(); ++i) {
    const double p = std::norm(H[i]);
    require(p > 1e-12, "ZF zero channel");
    x[i] = y[i] / H[i];
  }
  return x;
}

ComplexVec equalize_mmse_tones(const ComplexVec& y, const ComplexVec& H, double noise_var) {
  require(y.size() == H.size(), "MMSE size mismatch");
  require(noise_var >= 0, "noise_var");
  ComplexVec x(y.size());
  for (std::size_t i = 0; i < y.size(); ++i) {
    const double denom = std::norm(H[i]) + noise_var;
    x[i] = y[i] * std::conj(H[i]) / denom;
  }
  return x;
}

}  // namespace nr_bb
