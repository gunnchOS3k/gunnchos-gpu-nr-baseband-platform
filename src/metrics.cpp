#include "nr_bb/metrics.hpp"

namespace nr_bb {

void accumulate_ber(LinkMetrics& m, const BitVec& ref, const BitVec& hat) {
  require(ref.size() == hat.size(), "BER length mismatch");
  m.bit_total += ref.size();
  for (std::size_t i = 0; i < ref.size(); ++i)
    if (ref[i] != hat[i]) ++m.bit_errors;
}

void accumulate_bler(LinkMetrics& m, bool crc_ok) {
  ++m.block_total;
  if (!crc_ok) ++m.block_errors;
}

}  // namespace nr_bb
