#include <catch2/catch_test_macros.hpp>
#include "nr_bb/ofdm.hpp"
#include <cmath>

TEST_CASE("OFDM roundtrip", "[ofdm]") {
  nr_bb::OfdmConfig cfg{.fft_size = 32, .cp_len = 4};
  nr_bb::ComplexVec freq(cfg.fft_size);
  for (size_t i = 0; i < freq.size(); ++i) freq[i] = {std::sin(0.2 * i), std::cos(0.1 * i)};
  auto time = nr_bb::ofdm_modulate(freq, cfg);
  REQUIRE(time.size() == cfg.fft_size + cfg.cp_len);
  auto back = nr_bb::ofdm_demodulate(time, cfg);
  for (size_t i = 0; i < freq.size(); ++i) {
    REQUIRE(std::abs(back[i] - freq[i]) < 1e-9);
  }
}
