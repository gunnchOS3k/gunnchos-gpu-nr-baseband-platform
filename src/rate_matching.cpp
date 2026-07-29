#include "nr_bb/rate_matching.hpp"

namespace nr_bb {

BitVec rate_match(const BitVec& coded, std::size_t e_bits) {
  require(!coded.empty(), "rate_match empty input");
  require(e_bits > 0, "E must be > 0");
  BitVec out(e_bits);
  for (std::size_t i = 0; i < e_bits; ++i) out[i] = coded[i % coded.size()];
  return out;
}

SoftVec rate_dematch(const SoftVec& llr_e, std::size_t n_coded) {
  require(n_coded > 0, "n_coded must be > 0");
  SoftVec out(n_coded, 0.0);
  for (std::size_t i = 0; i < llr_e.size(); ++i) out[i % n_coded] += llr_e[i];
  return out;
}

}  // namespace nr_bb
