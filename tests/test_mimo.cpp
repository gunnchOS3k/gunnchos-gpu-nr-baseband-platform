#include <catch2/catch_test_macros.hpp>
#include "nr_bb/mimo.hpp"

TEST_CASE("MIMO SISO 2x2 4x4 identity high SNR", "[mimo]") {
  for (int n : {1, 2, 4}) {
    std::vector<nr_bb::ComplexVec> tx(static_cast<size_t>(n), nr_bb::ComplexVec(8, {0.5, -0.25}));
    auto ch = nr_bb::identity_mimo(n, 40.0, 7);
    nr_bb::EqualizerConfig eq;
    eq.type = nr_bb::EqualizerType::MMSE;
    eq.noise_var = 0.001;
    auto r = nr_bb::run_mimo_link(tx, ch, eq);
    REQUIRE(static_cast<int>(r.equalized.size()) == n);
    REQUIRE(r.equalized[0].size() == 8);
  }
}
