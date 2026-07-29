#include <catch2/catch_test_macros.hpp>
#include "nr_bb/channel_estimation.hpp"
#include <cmath>

TEST_CASE("LS channel estimate", "[channel_estimation]") {
  nr_bb::ComplexVec tx = {{1, 0}, {0, 1}};
  nr_bb::ComplexVec rx = {{2, 0}, {0, 2}};
  auto H = nr_bb::ls_channel_estimate(rx, tx);
  REQUIRE(std::abs(H[0] - nr_bb::Complex{2, 0}) < 1e-12);
}

TEST_CASE("ZF/MMSE tone equalizers", "[channel_estimation]") {
  nr_bb::ComplexVec H = {{2, 0}, {0, 2}};
  nr_bb::ComplexVec y = {{2, 0}, {0, 2}};
  auto xz = nr_bb::equalize_zf_tones(y, H);
  REQUIRE(std::abs(xz[0] - nr_bb::Complex{1, 0}) < 1e-12);
  auto xm = nr_bb::equalize_mmse_tones(y, H, 0.0);
  REQUIRE(std::abs(xm[0] - nr_bb::Complex{1, 0}) < 1e-12);
}

TEST_CASE("frequency interpolate", "[channel_estimation]") {
  nr_bb::ComplexVec hp = {{1, 0}, {3, 0}};
  std::vector<int> idx = {0, 2};
  auto H = nr_bb::interpolate_frequency(hp, idx, 3);
  REQUIRE(std::abs(H[1].real() - 2.0) < 1e-12);
}
