#include "nr_bb/segmentation.hpp"
#include <algorithm>

namespace nr_bb {

SegmentationResult segment_transport_block(const BitVec& tb, const SegmentationConfig& cfg) {
  SegmentationResult r;
  if (tb.empty()) return r;

  const std::size_t B = tb.size();
  const std::size_t K_cb = cfg.max_cb_info_bits;
  require(K_cb >= 40, "max_cb_info_bits too small");

  // Determine number of code blocks C (TS 38.212 §5.2.2 simplified).
  int C = 1;
  if (B > K_cb) {
    // CRC24B per CB when segmented
    const int L = cfg.attach_crc24b_when_segmented ? 24 : 0;
    C = static_cast<int>((B + static_cast<std::size_t>(L) * 0 + K_cb - 1) / K_cb);
    // Refine: B' = B + C*L, K' = ceil(B'/C)
    while (true) {
      const int Luse = (C > 1 && cfg.attach_crc24b_when_segmented) ? 24 : 0;
      const std::size_t Bp = B + static_cast<std::size_t>(C * Luse);
      const std::size_t Kp = (Bp + static_cast<std::size_t>(C) - 1) / static_cast<std::size_t>(C);
      if (Kp - static_cast<std::size_t>(Luse) <= K_cb) break;
      ++C;
      require(C < 10000, "segmentation C runaway");
    }
  }
  r.num_code_blocks = C;
  const int L = (C > 1 && cfg.attach_crc24b_when_segmented) ? 24 : 0;
  const std::size_t Bp = B + static_cast<std::size_t>(C * L);
  const std::size_t Kp = (Bp + static_cast<std::size_t>(C) - 1) / static_cast<std::size_t>(C);
  const std::size_t Kinfo = Kp - static_cast<std::size_t>(L);

  // Split TB into C segments of Kinfo (last may be shorter before filler).
  std::size_t offset = 0;
  for (int c = 0; c < C; ++c) {
    CodeBlockPlan plan;
    const std::size_t remain = B - offset;
    const std::size_t take = std::min(Kinfo, remain);
    BitVec payload(tb.begin() + static_cast<std::ptrdiff_t>(offset),
                   tb.begin() + static_cast<std::ptrdiff_t>(offset + take));
    // Pad payload to Kinfo with zeros before CRC (filler at CB level before lifting pad).
    if (payload.size() < Kinfo) payload.resize(Kinfo, 0);
    offset += take;

    if (L > 0) {
      plan.bits_with_crc = crc_attach(payload, CrcType::CRC24B);
    } else {
      plan.bits_with_crc = payload;
    }

    BaseGraph bg = cfg.bg;
    if (cfg.auto_select_bg) {
      bg = select_basegraph(static_cast<int>(plan.bits_with_crc.size()), cfg.target_rate);
    }
    auto lift = select_lifting(static_cast<int>(plan.bits_with_crc.size()), bg);
    plan.bg = bg;
    plan.zc = lift.zc;
    plan.i_ls = lift.i_ls;
    plan.k_ldpc = lift.kb * lift.zc;
    plan.filler_bits = plan.k_ldpc - static_cast<int>(plan.bits_with_crc.size());
    require(plan.filler_bits >= 0, "negative filler");
    plan.ldpc = LdpcParams{.bg = bg,
                           .zc = lift.zc,
                           .i_ls = lift.i_ls,
                           .info_bits = static_cast<int>(plan.bits_with_crc.size()),
                           .kb = lift.kb};
    r.total_filler_bits += static_cast<std::size_t>(plan.filler_bits);
    r.code_blocks.push_back(std::move(plan));
  }
  return r;
}

SegmentationResult segment_transport_block_fixed(const BitVec& tb, std::size_t max_cb_bits) {
  SegmentationConfig cfg;
  cfg.max_cb_info_bits = max_cb_bits;
  cfg.attach_crc24b_when_segmented = false;
  cfg.auto_select_bg = true;
  return segment_transport_block(tb, cfg);
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
