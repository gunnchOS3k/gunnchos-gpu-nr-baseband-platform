#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "nr_bb/rate_matching.hpp"

TEST_CASE("rate match puncture and repeat", "[rm]") {
  nr_bb::BitVec coded = {1, 0, 1, 1, 0, 0, 1, 0};
  auto e = nr_bb::rate_match(coded, 5);
  REQUIRE(e.size() == 5);
  auto e2 = nr_bb::rate_match(coded, 20);
  REQUIRE(e2.size() == 20);
  nr_bb::SoftVec llr(20, 1.0);
  auto d = nr_bb::rate_dematch(llr, 8);
  REQUIRE(d.size() == 8);
  REQUIRE(d[0] == Catch::Approx(3.0));
}
