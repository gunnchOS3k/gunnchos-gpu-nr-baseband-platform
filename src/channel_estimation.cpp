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

}  // namespace nr_bb
