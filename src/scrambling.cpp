#include "nr_bb/scrambling.hpp"

namespace nr_bb {

BitVec gold_sequence(std::size_t length, uint32_t c_init) {
  const std::size_t Nc = 1600;
  const std::size_t total = Nc + length;
  std::vector<uint8_t> X1(total + 31, 0), X2(total + 31, 0);
  X1[0] = 1;
  for (int i = 0; i < 31; ++i) X2[static_cast<size_t>(i)] = static_cast<uint8_t>((c_init >> i) & 1u);
  for (std::size_t n = 0; n < total; ++n) {
    X1[n + 31] = X1[n + 3] ^ X1[n];
    X2[n + 31] = X2[n + 3] ^ X2[n + 2] ^ X2[n + 1] ^ X2[n];
  }
  BitVec c(length);
  for (std::size_t n = 0; n < length; ++n) c[n] = X1[n + Nc] ^ X2[n + Nc];
  return c;
}

BitVec scramble(const BitVec& bits, uint32_t c_init) {
  const auto c = gold_sequence(bits.size(), c_init);
  BitVec out(bits.size());
  for (std::size_t i = 0; i < bits.size(); ++i) {
    require(bits[i] == 0 || bits[i] == 1, "scramble bits must be 0/1");
    out[i] = bits[i] ^ c[i];
  }
  return out;
}

BitVec descramble(const BitVec& bits, uint32_t c_init) { return scramble(bits, c_init); }

}  // namespace nr_bb
