#include <catch2/catch_test_macros.hpp>
#include "nr_bb/layer_mapping.hpp"

TEST_CASE("layer map demap 2 and 4", "[layer]") {
  nr_bb::ComplexVec sym(8);
  for (size_t i = 0; i < sym.size(); ++i) sym[i] = {double(i), -double(i)};
  auto L2 = nr_bb::layer_map(sym, 2);
  REQUIRE(L2.size() == 2);
  REQUIRE(nr_bb::layer_demap(L2) == sym);
  auto L4 = nr_bb::layer_map(sym, 4);
  REQUIRE(L4.size() == 4);
  REQUIRE(nr_bb::layer_demap(L4) == sym);
}
