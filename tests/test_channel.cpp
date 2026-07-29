#include <catch2/catch_test_macros.hpp>
#include "nr_bb/channel.hpp"

TEST_CASE("SISO channel length with delay", "[channel]") {
  nr_bb::ComplexVec tx(10, {1, 0});
  nr_bb::ChannelConfig cfg;
  cfg.snr_db = 40;
  cfg.timing_offset_samples = 3;
  cfg.cfo_hz = 100;
  auto rx = nr_bb::apply_siso_channel(tx, cfg);
  REQUIRE(rx.size() == 13);
}
