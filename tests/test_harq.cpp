#include <catch2/catch_test_macros.hpp>
#include "nr_bb/harq.hpp"

TEST_CASE("HARQ chase combine", "[harq]") {
  nr_bb::HarqBuffer buf;
  nr_bb::SoftVec a = {1.0, -1.0, 0.5};
  nr_bb::SoftVec b = {1.0, -0.5, 0.5};
  buf.upsert(1, 0, a, 0);
  buf.upsert(1, 0, b, 1);
  auto g = buf.get(1);
  REQUIRE(g.has_value());
  REQUIRE((*g)[0] == 2.0);
  buf.upsert(1, 1, a, 0);  // new NDI
  REQUIRE(buf.get(1).value()[0] == 1.0);
  buf.clear(1);
  REQUIRE_FALSE(buf.get(1).has_value());
}
