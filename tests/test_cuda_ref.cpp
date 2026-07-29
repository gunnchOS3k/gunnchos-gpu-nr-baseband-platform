#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
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

TEST_CASE("CUDA CPU ref ZF/MMSE/HARQ", "[cuda_ref]") {
  std::vector<float> y_re = {1.f, 0.5f}, y_im = {0.f, -0.25f};
  std::vector<float> h_re = {1.f, 1.f}, h_im = {0.f, 0.f};
  std::vector<float> x_re, x_im;
  nr_bb::cuda_ref::zf_tone_f32(y_re, y_im, h_re, h_im, x_re, x_im);
  REQUIRE(x_re[0] == Catch::Approx(1.f).margin(1e-5));
  nr_bb::cuda_ref::mmse_tone_f32(y_re, y_im, h_re, h_im, 0.01f, x_re, x_im);
  REQUIRE(x_re.size() == 2);
  auto comb = nr_bb::cuda_ref::harq_combine_f32({1.f, 2.f}, {3.f, 4.f});
  REQUIRE(comb[0] == Catch::Approx(4.f));
  REQUIRE(comb[1] == Catch::Approx(6.f));
}
