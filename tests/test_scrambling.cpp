#include <catch2/catch_test_macros.hpp>
#include "nr_bb/scrambling.hpp"

TEST_CASE("scramble involution", "[scrambling]") {
  nr_bb::BitVec bits = {1, 0, 1, 1, 0, 0, 1, 0, 1, 1};
  auto s = nr_bb::scramble(bits, 0x1234);
  auto d = nr_bb::descramble(s, 0x1234);
  REQUIRE(d == bits);
  REQUIRE(s != bits);
}
