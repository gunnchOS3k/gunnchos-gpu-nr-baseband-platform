#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "nr_bb/metrics.hpp"

TEST_CASE("BER BLER accumulate", "[metrics]") {
  nr_bb::LinkMetrics m;
  nr_bb::accumulate_ber(m, {0, 1, 1}, {0, 0, 1});
  REQUIRE(m.bit_errors == 1);
  nr_bb::accumulate_bler(m, false);
  nr_bb::accumulate_bler(m, true);
  REQUIRE(m.bler() == Catch::Approx(0.5));
}
