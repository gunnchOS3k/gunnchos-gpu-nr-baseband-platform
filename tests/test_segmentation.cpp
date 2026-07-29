#include <catch2/catch_test_macros.hpp>
#include "nr_bb/segmentation.hpp"

TEST_CASE("segmentation roundtrip", "[segmentation]") {
  nr_bb::BitVec tb(200, 1);
  nr_bb::SegmentationConfig cfg{.max_cb_bits = 64};
  auto seg = nr_bb::segment_transport_block(tb, cfg);
  REQUIRE(seg.code_blocks.size() == 4);
  auto back = nr_bb::concatenate_code_blocks(seg.code_blocks, seg.filler_bits);
  REQUIRE(back == tb);
}

TEST_CASE("segmentation rejects tiny CB", "[segmentation]") {
  nr_bb::SegmentationConfig cfg{.max_cb_bits = 2};
  REQUIRE_THROWS_AS(nr_bb::segment_transport_block({1, 0, 1}, cfg), nr_bb::Error);
}
