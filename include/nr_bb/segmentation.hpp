#pragma once
/**
 * @file segmentation.hpp
 * @brief NR-oriented code-block segmentation with CRC24B and filler bits.
 *
 * TRACEABILITY: TS 38.212 §5.2.1 (simplified single/multi-CB path).
 * When C>1, CRC24B is attached per code block. Filler bits pad to K = kb*Zc.
 */
#include "nr_bb/crc.hpp"
#include "nr_bb/ldpc.hpp"
#include "nr_bb/types.hpp"
#include <cstddef>
#include <vector>

namespace nr_bb {

struct SegmentationConfig {
  std::size_t max_cb_info_bits = 8448;  // K_cb upper bound (BG1)
  bool attach_crc24b_when_segmented = true;
  BaseGraph bg = BaseGraph::BG2;
  double target_rate = 0.5;  // used only for BG auto-select if bg forced via params
  bool auto_select_bg = true;
};

struct CodeBlockPlan {
  BitVec bits_with_crc;  // payload (+ CRC24B if C>1)
  int filler_bits = 0;
  int k_ldpc = 0;
  int zc = 0;
  int i_ls = 0;
  BaseGraph bg = BaseGraph::BG2;
  LdpcParams ldpc;
};

struct SegmentationResult {
  std::vector<CodeBlockPlan> code_blocks;
  std::size_t transport_crc_bits = 0;  // if TB CRC already included by caller: 0
  std::size_t total_filler_bits = 0;
  int num_code_blocks = 0;
};

SegmentationResult segment_transport_block(const BitVec& tb, const SegmentationConfig& cfg);

/** Legacy helper retained for educational/simple paths. */
SegmentationResult segment_transport_block_fixed(const BitVec& tb, std::size_t max_cb_bits);

BitVec concatenate_code_blocks(const std::vector<BitVec>& cbs, std::size_t filler_bits);

}  // namespace nr_bb
