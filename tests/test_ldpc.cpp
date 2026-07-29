#include <catch2/catch_test_macros.hpp>
#include "nr_bb/ldpc.hpp"
#include "nr_bb/llr.hpp"

TEST_CASE("NR LDPC BG2 encode syndrome + noiseless decode", "[ldpc][standards]") {
  nr_bb::LdpcParams p{.bg = nr_bb::BaseGraph::BG2, .zc = 2};
  const auto g = nr_bb::ldpc_graph_info(p);
  REQUIRE(g.kb == 4);
  REQUIRE(g.zc == 2);
  nr_bb::BitVec info(static_cast<size_t>(g.kb * g.zc));
  for (size_t i = 0; i < info.size(); ++i) info[i] = static_cast<uint8_t>((i * 3) & 1);
  auto cw = nr_bb::ldpc_encode(info, p);
  REQUIRE(cw.size() == static_cast<size_t>(g.nb * g.zc));
  REQUIRE(nr_bb::ldpc_syndrome_ok(cw, p));
  auto llr = nr_bb::bits_to_llr(cw, 12.0);
  auto hat = nr_bb::ldpc_decode(llr, p);
  REQUIRE(hat == info);
}

TEST_CASE("NR LDPC BG1 scaffold encode", "[ldpc][standards]") {
  nr_bb::LdpcParams p{.bg = nr_bb::BaseGraph::BG1, .zc = 2};
  const auto g = nr_bb::ldpc_graph_info(p);
  nr_bb::BitVec info(static_cast<size_t>(g.kb * g.zc), 0);
  info[0] = 1;
  info[3] = 1;
  auto ref = nr_bb::ldpc_make_reference(info, p);
  REQUIRE(ref.codeword.size() == static_cast<size_t>(g.nb * g.zc));
  REQUIRE(ref.provenance.find("BG1") != std::string::npos);
  REQUIRE(nr_bb::ldpc_syndrome_ok(ref.codeword, p));
}

TEST_CASE("NR LDPC rejects wrong info length", "[ldpc]") {
  nr_bb::LdpcParams p{.bg = nr_bb::BaseGraph::BG2, .zc = 2};
  REQUIRE_THROWS_AS(nr_bb::ldpc_encode({1, 0, 1}, p), nr_bb::Error);
}
