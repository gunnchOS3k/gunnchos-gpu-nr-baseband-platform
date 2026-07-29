#include "nr_bb/rate_matching.hpp"
#include <vector>

namespace nr_bb {

std::vector<std::size_t> rv_k0_positions(std::size_t n_cb, int zc, BaseGraph bg) {
  require(zc > 0, "zc required for RV k0");
  require(n_cb > 0, "n_cb required");
  // TS 38.212 Table 5.4.2.1-2 coefficients (I_LBRM=0 ⇒ N_cb = N).
  const int coeffs_bg1[4] = {0, 17, 33, 56};
  const int coeffs_bg2[4] = {0, 13, 25, 43};
  const int* coeffs = (bg == BaseGraph::BG1) ? coeffs_bg1 : coeffs_bg2;
  const int n_cols = (bg == BaseGraph::BG1) ? 66 : 50;
  std::vector<std::size_t> out(4);
  for (int rv = 0; rv < 4; ++rv) {
    out[static_cast<size_t>(rv)] =
        static_cast<std::size_t>((coeffs[rv] * static_cast<int>(n_cb) / (n_cols * zc)) * zc);
  }
  return out;
}

namespace {
std::size_t start_offset(const RateMatchConfig& cfg, std::size_t n_cb) {
  if (cfg.zc > 0) {
    auto k0s = rv_k0_positions(n_cb, cfg.zc, cfg.bg);
    const int r = ((cfg.rv % 4) + 4) % 4;
    return k0s[static_cast<size_t>(r)] % n_cb;
  }
  // Fallback educational offsets when zc unknown.
  const int r = ((cfg.rv % 4) + 4) % 4;
  return (n_cb * static_cast<std::size_t>(r)) / 4;
}
}  // namespace

BitVec rate_match(const BitVec& coded, std::size_t e_bits) {
  RateMatchConfig cfg;
  cfg.e_bits = e_bits;
  return rate_match(coded, cfg);
}

BitVec rate_match(const BitVec& coded, const RateMatchConfig& cfg) {
  require(!coded.empty(), "rate_match empty input");
  require(cfg.e_bits > 0, "E must be > 0");

  BitVec systematic = coded;
  if (cfg.puncture_first_2z && cfg.zc > 0) {
    const std::size_t punch = static_cast<std::size_t>(2 * cfg.zc);
    require(coded.size() > punch, "coded too short to puncture 2Z");
    systematic.assign(coded.begin() + static_cast<std::ptrdiff_t>(punch), coded.end());
  }

  const std::size_t n_cb = cfg.n_cb == 0 ? systematic.size() : cfg.n_cb;
  require(n_cb > 0, "n_cb must be > 0");
  BitVec buf(n_cb, 0);
  for (std::size_t i = 0; i < n_cb; ++i) buf[i] = systematic[i % systematic.size()];
  const std::size_t k0 = start_offset(cfg, n_cb);
  BitVec out(cfg.e_bits);
  for (std::size_t i = 0; i < cfg.e_bits; ++i) out[i] = buf[(k0 + i) % n_cb];
  return out;
}

SoftVec rate_dematch(const SoftVec& llr_e, std::size_t n_coded) {
  RateMatchConfig cfg;
  cfg.e_bits = llr_e.size();
  cfg.puncture_first_2z = false;
  return rate_dematch(llr_e, cfg, n_coded);
}

SoftVec rate_dematch(const SoftVec& llr_e, const RateMatchConfig& cfg, std::size_t n_coded) {
  require(n_coded > 0, "n_coded must be > 0");
  SoftVec full(n_coded, 0.0);

  std::size_t punch = 0;
  if (cfg.puncture_first_2z && cfg.zc > 0) {
    punch = static_cast<std::size_t>(2 * cfg.zc);
    require(n_coded > punch, "n_coded too short for 2Z puncture");
  }
  const std::size_t n_cb = cfg.n_cb == 0 ? (n_coded - punch) : cfg.n_cb;
  SoftVec buf(n_cb, 0.0);
  const std::size_t k0 = start_offset(cfg, n_cb);
  for (std::size_t i = 0; i < llr_e.size(); ++i) buf[(k0 + i) % n_cb] += llr_e[i];
  for (std::size_t i = 0; i < n_cb; ++i) full[punch + (i % (n_coded - punch))] = buf[i];
  return full;
}

}  // namespace nr_bb
