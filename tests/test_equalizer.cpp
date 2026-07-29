#include <catch2/catch_test_macros.hpp>
#include "nr_bb/equalizer.hpp"
#include <cmath>

TEST_CASE("ZF recovers identity 2x2", "[eq]") {
  std::vector<nr_bb::Complex> H = {{1, 0}, {0, 0}, {0, 0}, {1, 0}};
  nr_bb::ComplexVec y = {{0.7, 0.1}, {-0.2, 0.3}};
  nr_bb::EqualizerConfig cfg;
  cfg.type = nr_bb::EqualizerType::ZF;
  auto x = nr_bb::equalize_sample(y, H, 2, 2, cfg);
  REQUIRE(std::abs(x[0] - y[0]) < 1e-9);
  REQUIRE(std::abs(x[1] - y[1]) < 1e-9);
}

TEST_CASE("MMSE and RZF run on 4x4", "[eq]") {
  const int n = 4;
  std::vector<nr_bb::Complex> H(n * n, {0, 0});
  for (int i = 0; i < n; ++i) H[static_cast<size_t>(i * n + i)] = {1, 0};
  nr_bb::ComplexVec y(n, {0.1, -0.2});
  for (auto type : {nr_bb::EqualizerType::MMSE, nr_bb::EqualizerType::RZF}) {
    nr_bb::EqualizerConfig cfg;
    cfg.type = type;
    cfg.noise_var = 0.05;
    cfg.rzf_delta = 0.05;
    auto x = nr_bb::equalize_sample(y, H, n, n, cfg);
    REQUIRE(x.size() == static_cast<size_t>(n));
  }
}
