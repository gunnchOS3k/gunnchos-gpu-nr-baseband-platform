#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include "nr_bb/modulation.hpp"
#include <cmath>
#include <random>

TEST_CASE("Gray constellation unit energy", "[modulation]") {
  for (auto order : {nr_bb::ModulationOrder::QPSK, nr_bb::ModulationOrder::QAM16,
                     nr_bb::ModulationOrder::QAM64, nr_bb::ModulationOrder::QAM256}) {
    REQUIRE(nr_bb::constellation_avg_energy(order) == Catch::Approx(1.0).margin(1e-9));
  }
}

TEST_CASE("modulation hard roundtrip Gray QPSK/16/64/256", "[modulation]") {
  for (auto order : {nr_bb::ModulationOrder::QPSK, nr_bb::ModulationOrder::QAM16,
                     nr_bb::ModulationOrder::QAM64, nr_bb::ModulationOrder::QAM256}) {
    const int m = nr_bb::bits_per_symbol(order);
    nr_bb::BitVec bits;
    for (int i = 0; i < m * 8; ++i) bits.push_back(static_cast<uint8_t>((i * 3 + 1) & 1));
    auto sym = nr_bb::modulate(bits, order);
    auto hat = nr_bb::demodulate_hard(sym, order);
    REQUIRE(hat == bits);
  }
}

TEST_CASE("soft LLR max-log recovers bits under light noise", "[modulation]") {
  std::mt19937 rng(7);
  std::normal_distribution<double> n(0.0, 0.05);
  nr_bb::BitVec bits;
  for (int i = 0; i < 64; ++i) bits.push_back(static_cast<uint8_t>((i * 5) & 1));
  auto tx = nr_bb::modulate(bits, nr_bb::ModulationOrder::QAM16);
  auto rx = tx;
  for (auto& z : rx) z += nr_bb::Complex{n(rng), n(rng)};
  auto llr = nr_bb::demodulate_llr(rx, nr_bb::ModulationOrder::QAM16, 0.01);
  REQUIRE(llr.size() == bits.size());
  int err = 0;
  for (size_t i = 0; i < bits.size(); ++i) {
    const uint8_t hat = (llr[i] >= 0) ? 0 : 1;
    if (hat != bits[i]) ++err;
  }
  REQUIRE(err == 0);
}

TEST_CASE("QPSK Gray mapping known points", "[modulation]") {
  // bits 00 → (+1,+1)/√2
  nr_bb::BitVec b00 = {0, 0};
  auto s = nr_bb::modulate(b00, nr_bb::ModulationOrder::QPSK);
  REQUIRE(s.size() == 1);
  REQUIRE(s[0].real() == Catch::Approx(1.0 / std::sqrt(2.0)));
  REQUIRE(s[0].imag() == Catch::Approx(1.0 / std::sqrt(2.0)));
}
