#include <catch2/catch_test_macros.hpp>
#include "nr_bb/ldpc.hpp"
#include "nr_bb/llr.hpp"

TEST_CASE("LDPC encode/decode noiseless", "[ldpc]") {
  nr_bb::BitVec info = {1, 0, 1, 1, 0, 0, 1, 0};
  auto cw = nr_bb::ldpc_encode(info);
  REQUIRE(cw.size() == 16);
  auto llr = nr_bb::bits_to_llr(cw, 8.0);
  auto hat = nr_bb::ldpc_decode(llr);
  REQUIRE(hat == info);
}

TEST_CASE("LDPC reference vector", "[ldpc]") {
  nr_bb::BitVec info(8, 0);
  auto ref = nr_bb::ldpc_make_reference(info);
  REQUIRE(ref.codeword.size() == 16);
  REQUIRE(ref.provenance.find("self-generated") != std::string::npos);
}

TEST_CASE("LDPC rejects wrong length", "[ldpc]") {
  REQUIRE_THROWS_AS(nr_bb::ldpc_encode({1, 0, 1}), nr_bb::Error);
}
