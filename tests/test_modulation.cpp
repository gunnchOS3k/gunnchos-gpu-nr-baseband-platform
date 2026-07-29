#include <catch2/catch_test_macros.hpp>
#include "nr_bb/modulation.hpp"

TEST_CASE("modulation roundtrip QPSK/16/64/256", "[modulation]") {
  for (auto order : {nr_bb::ModulationOrder::QPSK, nr_bb::ModulationOrder::QAM16,
                     nr_bb::ModulationOrder::QAM64, nr_bb::ModulationOrder::QAM256}) {
    const int m = nr_bb::bits_per_symbol(order);
    nr_bb::BitVec bits;
    for (int i = 0; i < m * 4; ++i) bits.push_back(static_cast<uint8_t>((i * 3) & 1));
    auto sym = nr_bb::modulate(bits, order);
    auto hat = nr_bb::demodulate_hard(sym, order);
    REQUIRE(hat == bits);
  }
}

TEST_CASE("modulation LLR length", "[modulation]") {
  nr_bb::BitVec bits = {0, 1, 1, 0};
  auto sym = nr_bb::modulate(bits, nr_bb::ModulationOrder::QPSK);
  auto llr = nr_bb::demodulate_llr(sym, nr_bb::ModulationOrder::QPSK, 0.1);
  REQUIRE(llr.size() == bits.size());
}
