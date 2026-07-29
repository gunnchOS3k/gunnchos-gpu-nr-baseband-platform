#pragma once
/**
 * @file segmentation.hpp
 * @brief Code-block segmentation helpers (educational).
 *
 * TRACEABILITY NOTE: Inspired by 3GPP TS 38.212 §5.2.1 code-block segmentation.
 * Uses a simplified fixed CB size for the educational path; NOT full 38.212 TB/CB
 * segmentation conformance.
 */
#include "nr_bb/types.hpp"
#include <cstddef>
#include <vector>

namespace nr_bb {

struct SegmentationConfig {
  std::size_t max_cb_bits = 128;  // educational short-block default
};

struct SegmentationResult {
  std::vector<BitVec> code_blocks;
  std::size_t filler_bits = 0;
};

SegmentationResult segment_transport_block(const BitVec& tb, const SegmentationConfig& cfg);
BitVec concatenate_code_blocks(const std::vector<BitVec>& cbs, std::size_t filler_bits);

}  // namespace nr_bb
