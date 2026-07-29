#include <catch2/catch_test_macros.hpp>
#include "nr_bb/cuda_cpu_ref.hpp"
#include "nr_bb/scrambling.hpp"

TEST_CASE("CUDA CPU ref scramble/rate-match/mod", "[cuda_ref]") {
  nr_bb::BitVec bits = {0, 1, 1, 0, 1, 0, 0, 1};
  auto scrambled = nr_bb::cuda_ref::scramble_bits(bits, 0xABCDu);
  REQUIRE(scrambled == nr_bb::scramble(bits, 0xABCDu));
  auto rm = nr_bb::cuda_ref::rate_match_bits(bits, 12);
  REQUIRE(rm.size() == 12);
  auto iq = nr_bb::cuda_ref::modulate_qpsk_f32(bits);
  REQUIRE(iq.size() == 8);
  auto llr = nr_bb::cuda_ref::qpsk_llr_f32(iq, 0.1f);
  REQUIRE(llr.size() == 8);
  auto cmp = nr_bb::cuda_ref::compare_f32(iq, iq, "self");
  REQUIRE(cmp.match);
}
