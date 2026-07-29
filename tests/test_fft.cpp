#include <catch2/catch_test_macros.hpp>
#include "nr_bb/fft.hpp"
#include <cmath>

TEST_CASE("FFT roundtrip", "[fft]") {
  nr_bb::ComplexVec x(32);
  for (size_t i = 0; i < x.size(); ++i) x[i] = {std::sin(0.2 * i), std::cos(0.1 * i)};
  auto X = nr_bb::fft_radix2(x, false);
  auto back = nr_bb::fft_radix2(X, true);
  for (size_t i = 0; i < x.size(); ++i) REQUIRE(std::abs(back[i] - x[i]) < 1e-9);
}

TEST_CASE("FFT rejects non power of two", "[fft]") {
  nr_bb::ComplexVec x(3, {1, 0});
  REQUIRE_THROWS_AS(nr_bb::fft_radix2(x, false), nr_bb::Error);
}
