#include "nr_bb/segmentation.hpp"

namespace nr_bb {

SegmentationResult segment_transport_block(const BitVec& tb, const SegmentationConfig& cfg) {
  require(cfg.max_cb_bits >= 8, "max_cb_bits too small");
  SegmentationResult r;
  if (tb.empty()) return r;
  const std::size_t n_cb = (tb.size() + cfg.max_cb_bits - 1) / cfg.max_cb_bits;
  r.code_blocks.resize(n_cb);
  std::size_t offset = 0;
  for (std::size_t i = 0; i < n_cb; ++i) {
    const std::size_t take = std::min(cfg.max_cb_bits, tb.size() - offset);
    r.code_blocks[i].assign(tb.begin() + static_cast<std::ptrdiff_t>(offset),
                            tb.begin() + static_cast<std::ptrdiff_t>(offset + take));
    if (take < cfg.max_cb_bits) {
      const std::size_t fill = cfg.max_cb_bits - take;
      r.code_blocks[i].insert(r.code_blocks[i].end(), fill, 0);
      r.filler_bits += fill;
    }
    offset += take;
  }
  return r;
}

BitVec concatenate_code_blocks(const std::vector<BitVec>& cbs, std::size_t filler_bits) {
  BitVec out;
  for (const auto& cb : cbs) out.insert(out.end(), cb.begin(), cb.end());
  if (filler_bits > 0) {
    require(out.size() >= filler_bits, "filler exceeds concatenated size");
    out.resize(out.size() - filler_bits);
  }
  return out;
}

}  // namespace nr_bb
