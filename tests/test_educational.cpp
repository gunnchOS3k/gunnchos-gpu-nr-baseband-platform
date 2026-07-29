#include <catch2/catch_test_macros.hpp>
#include "nr_bb_edu/ldpc_short.hpp"
#include "nr_bb_edu/naive_dft.hpp"
#include "nr_bb_edu/binary_pam.hpp"
#include "nr_bb/fft.hpp"
#include <cmath>

TEST_CASE("educational short LDPC still works labeled", "[educational]") {
  nr_bb::BitVec info = {1, 0, 1, 1, 0, 0, 1, 0};
  auto cw = nr_bb_edu::ldpc_encode(info);
  REQUIRE(cw.size() == 16);
  auto ref = nr_bb_edu::make_reference(info);
  REQUIRE(ref.provenance.find("EDUCATIONAL") != std::string::npos);
}

TEST_CASE("educational naive DFT vs acceptance FFT", "[educational]") {
  nr_bb::ComplexVec x(16);
  for (size_t i = 0; i < x.size(); ++i) x[i] = {0.1 * i, -0.05};
  auto A = nr_bb_edu::naive_dft(x, false);
  auto B = nr_bb::fft_radix2(x, false);
  for (size_t i = 0; i < x.size(); ++i) REQUIRE(std::abs(A[i] - B[i]) < 1e-9);
}

TEST_CASE("educational binary PAM is separate from Gray path", "[educational]") {
  nr_bb::BitVec bits = {0, 0, 0, 1, 1, 0, 1, 1};
  auto edu = nr_bb_edu::modulate_binary_pam(bits, nr_bb::ModulationOrder::QAM16);
  REQUIRE(edu.size() == 2);
  auto llr = nr_bb_edu::hard_constant_llr(edu, nr_bb::ModulationOrder::QAM16, 5.0);
  REQUIRE(llr.size() == bits.size());
}
