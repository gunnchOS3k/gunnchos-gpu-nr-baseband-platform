#include <catch2/catch_test_macros.hpp>
#include "nr_bb/segmentation.hpp"

TEST_CASE("segmentation plans lifting and filler", "[segmentation]") {
  nr_bb::BitVec tb(200, 1);
  nr_bb::SegmentationConfig cfg;
  cfg.max_cb_info_bits = 8448;
  cfg.attach_crc24b_when_segmented = false;
  cfg.target_rate = 0.5;
  auto seg = nr_bb::segment_transport_block(tb, cfg);
  REQUIRE(seg.num_code_blocks == 1);
  REQUIRE(seg.code_blocks.size() == 1);
  REQUIRE(seg.code_blocks[0].bits_with_crc.size() == 200);
  REQUIRE(seg.code_blocks[0].filler_bits >= 0);
  REQUIRE(seg.code_blocks[0].k_ldpc >= 200);
}

TEST_CASE("segmentation rejects tiny CB bound", "[segmentation]") {
  nr_bb::SegmentationConfig cfg;
  cfg.max_cb_info_bits = 2;
  REQUIRE_THROWS_AS(nr_bb::segment_transport_block({1, 0, 1}, cfg), nr_bb::Error);
}
