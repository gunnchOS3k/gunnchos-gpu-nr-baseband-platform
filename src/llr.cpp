#include "nr_bb/llr.hpp"

namespace nr_bb {

SoftVec bits_to_llr(const BitVec& bits, double magnitude) {
  SoftVec out(bits.size());
  for (std::size_t i = 0; i < bits.size(); ++i) {
    require(bits[i] == 0 || bits[i] == 1, "bits 0/1");
    out[i] = (bits[i] == 0) ? magnitude : -magnitude;
  }
  return out;
}

BitVec llr_to_bits(const SoftVec& llr) {
  BitVec out(llr.size());
  for (std::size_t i = 0; i < llr.size(); ++i) out[i] = (llr[i] >= 0) ? 0 : 1;
  return out;
}

SoftVec llr_combine(const SoftVec& a, const SoftVec& b) {
  require(a.size() == b.size(), "llr_combine size mismatch");
  SoftVec out(a.size());
  for (std::size_t i = 0; i < a.size(); ++i) out[i] = a[i] + b[i];
  return out;
}

}  // namespace nr_bb
