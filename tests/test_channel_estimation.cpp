#include <catch2/catch_test_macros.hpp>
#include "nr_bb/channel_estimation.hpp"
#include <cmath>

TEST_CASE("LS channel estimate", "[chest]") {
  nr_bb::ComplexVec x = {{1, 0}, {0, 1}, {-1, 0}};
  nr_bb::Complex h = {0.5, -0.25};
  nr_bb::ComplexVec y;
  for (auto& xi : x) y.push_back(h * xi);
  auto he = nr_bb::ls_channel_estimate(y, x);
  for (auto& v : he) REQUIRE(std::abs(v - h) < 1e-12);
  auto grid = nr_bb::hold_interpolate(he, 2);
  REQUIRE(grid.size() == 2);
}
