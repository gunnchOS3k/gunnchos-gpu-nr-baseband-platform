#include <catch2/catch_test_macros.hpp>
#include "nr_bb/ber_campaign.hpp"

TEST_CASE("BER campaign runs and ber decreases with SNR", "[ber]") {
  auto r = nr_bb::run_ber_campaign(nr_bb::ModulationOrder::QPSK, {0.0, 12.0}, 512, 3);
  REQUIRE(r.points.size() == 2);
  REQUIRE(r.points[1].ber <= r.points[0].ber);
}
