#include "nr_bb/rate_matching.hpp"

namespace nr_bb {

namespace {
std::size_t start_offset(std::size_t n_cb, int rv) {
  // Educational RV offsets: 0, N/4, N/2, 3N/4 (scaffolding; not full 38.212 k0 table)
  const int r = ((rv % 4) + 4) % 4;
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
  const std::size_t n_cb = cfg.n_cb == 0 ? coded.size() : cfg.n_cb;
  require(n_cb > 0, "n_cb must be > 0");
  BitVec buf(n_cb, 0);
  for (std::size_t i = 0; i < n_cb; ++i) buf[i] = coded[i % coded.size()];
  const std::size_t k0 = start_offset(n_cb, cfg.rv);
  BitVec out(cfg.e_bits);
  for (std::size_t i = 0; i < cfg.e_bits; ++i) out[i] = buf[(k0 + i) % n_cb];
  return out;
}

SoftVec rate_dematch(const SoftVec& llr_e, std::size_t n_coded) {
  RateMatchConfig cfg;
  cfg.e_bits = llr_e.size();
  return rate_dematch(llr_e, cfg, n_coded);
}

SoftVec rate_dematch(const SoftVec& llr_e, const RateMatchConfig& cfg, std::size_t n_coded) {
  require(n_coded > 0, "n_coded must be > 0");
  const std::size_t n_cb = cfg.n_cb == 0 ? n_coded : cfg.n_cb;
  SoftVec buf(n_cb, 0.0);
  const std::size_t k0 = start_offset(n_cb, cfg.rv);
  for (std::size_t i = 0; i < llr_e.size(); ++i) buf[(k0 + i) % n_cb] += llr_e[i];
  SoftVec out(n_coded, 0.0);
  for (std::size_t i = 0; i < n_coded; ++i) out[i] = buf[i % n_cb];
  return out;
}

}  // namespace nr_bb
