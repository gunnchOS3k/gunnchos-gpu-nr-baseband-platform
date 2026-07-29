#include <catch2/catch_test_macros.hpp>
#include "nr_bb/llr.hpp"

TEST_CASE("LLR convert and combine", "[llr]") {
  nr_bb::BitVec bits = {0, 1, 0, 1};
  auto llr = nr_bb::bits_to_llr(bits, 2.0);
  REQUIRE(nr_bb::llr_to_bits(llr) == bits);
  auto c = nr_bb::llr_combine(llr, llr);
  REQUIRE(c[0] == 4.0);
}
