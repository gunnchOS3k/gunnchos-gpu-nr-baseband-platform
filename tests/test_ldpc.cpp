#include <catch2/catch_test_macros.hpp>
#include "nr_bb/ldpc.hpp"
#include "nr_bb/llr.hpp"
#include "nr_bb/harq.hpp"
#include "nr_bb/rate_matching.hpp"
#include "nr_bb/segmentation.hpp"

TEST_CASE("BG selection rules TS 38.212", "[ldpc][standards]") {
  REQUIRE(nr_bb::select_basegraph(100, 0.5) == nr_bb::BaseGraph::BG2);
  REQUIRE(nr_bb::select_basegraph(300, 0.5) == nr_bb::BaseGraph::BG2);
  REQUIRE(nr_bb::select_basegraph(4000, 0.8) == nr_bb::BaseGraph::BG1);
  REQUIRE(nr_bb::select_basegraph(4000, 0.2) == nr_bb::BaseGraph::BG2);
}

TEST_CASE("Zc lifting set membership", "[ldpc][standards]") {
  REQUIRE(nr_bb::is_valid_lifting_size(2));
  REQUIRE(nr_bb::is_valid_lifting_size(384));
  REQUIRE(nr_bb::is_valid_lifting_size(208));
  REQUIRE_FALSE(nr_bb::is_valid_lifting_size(17));
  REQUIRE(nr_bb::i_ls_for_zc(16) == 0);
  REQUIRE(nr_bb::i_ls_for_zc(3) == 1);
}

TEST_CASE("select_lifting chooses minimal Zc covering K", "[ldpc][standards]") {
  auto L = nr_bb::select_lifting(20, nr_bb::BaseGraph::BG2);
  REQUIRE(L.zc >= 2);
  REQUIRE(L.kb == 10);
  REQUIRE(L.kb * L.zc >= 20);
  auto L1 = nr_bb::select_lifting(500, nr_bb::BaseGraph::BG1);
  REQUIRE(L1.kb == 22);
  REQUIRE(L1.kb * L1.zc >= 500);
}

TEST_CASE("NR LDPC BG2 real graph encode syndrome + noiseless decode", "[ldpc][standards]") {
  nr_bb::LdpcParams p{.bg = nr_bb::BaseGraph::BG2, .zc = 2, .mb_use = 8};
  const auto g = nr_bb::ldpc_graph_info(p);
  REQUIRE(g.kb == 10);
  REQUIRE(g.nb == 18);  // kb + mb_use rate-matched subgraph
  REQUIRE(g.zc == 2);
  REQUIRE(g.provenance.find("BG2") != std::string::npos);
  nr_bb::BitVec info(static_cast<size_t>(g.kb * g.zc));
  for (size_t i = 0; i < info.size(); ++i) info[i] = static_cast<uint8_t>((i * 3) & 1);
  auto cw = nr_bb::ldpc_encode(info, p);
  REQUIRE(cw.size() == static_cast<size_t>(g.nb * g.zc));
  REQUIRE(nr_bb::ldpc_syndrome_ok(cw, p));
  auto llr = nr_bb::bits_to_llr(cw, 12.0);
  auto hat = nr_bb::ldpc_decode(llr, p);
  REQUIRE(hat == info);
}

TEST_CASE("NR LDPC BG1 real graph encode + syndrome", "[ldpc][standards]") {
  nr_bb::LdpcParams p{.bg = nr_bb::BaseGraph::BG1, .zc = 2, .mb_use = 8};
  const auto g = nr_bb::ldpc_graph_info(p);
  REQUIRE(g.kb == 22);
  REQUIRE(g.nb == 30);
  nr_bb::BitVec info(static_cast<size_t>(g.kb * g.zc), 0);
  info[0] = 1;
  info[5] = 1;
  auto ref = nr_bb::ldpc_make_reference(info, p);
  REQUIRE(ref.codeword.size() == static_cast<size_t>(g.nb * g.zc));
  REQUIRE(nr_bb::ldpc_syndrome_ok(ref.codeword, p));
}

TEST_CASE("NR LDPC full BG2 dimensions available", "[ldpc][standards]") {
  nr_bb::LdpcParams p{.bg = nr_bb::BaseGraph::BG2, .zc = 2};
  const auto g = nr_bb::ldpc_graph_info(p);
  REQUIRE(g.mb == 42);
  REQUIRE(g.nb == 52);
  REQUIRE(g.kb == 10);
}

TEST_CASE("NR LDPC filler bits shortened encode", "[ldpc][standards]") {
  nr_bb::LdpcParams p{.bg = nr_bb::BaseGraph::BG2, .zc = 4, .info_bits = 30, .mb_use = 6};
  nr_bb::BitVec info(30);
  for (size_t i = 0; i < info.size(); ++i) info[i] = static_cast<uint8_t>(i & 1);
  auto cw = nr_bb::ldpc_encode(info, p);
  REQUIRE(nr_bb::ldpc_syndrome_ok(cw, p));
  auto g = nr_bb::ldpc_graph_info(p);
  REQUIRE(g.filler_bits == g.k_ldpc - 30);
}

TEST_CASE("NR LDPC rejects overlong info", "[ldpc]") {
  nr_bb::LdpcParams p{.bg = nr_bb::BaseGraph::BG2, .zc = 2, .mb_use = 4};
  nr_bb::BitVec too_long(100, 1);
  REQUIRE_THROWS_AS(nr_bb::ldpc_encode(too_long, p), nr_bb::Error);
}

TEST_CASE("rate match RV k0 positions differ", "[rm][standards]") {
  auto k0 = nr_bb::rv_k0_positions(1000, 8, nr_bb::BaseGraph::BG2);
  REQUIRE(k0.size() == 4);
  REQUIRE(k0[0] == 0);
  REQUIRE(k0[1] != k0[2]);
}

TEST_CASE("HARQ soft combine with RM RVs", "[harq][ldpc]") {
  nr_bb::LdpcParams p{.bg = nr_bb::BaseGraph::BG2, .zc = 2, .mb_use = 6};
  auto g = nr_bb::ldpc_graph_info(p);
  nr_bb::BitVec info(static_cast<size_t>(g.k_ldpc), 0);
  for (size_t i = 0; i < info.size(); ++i) info[i] = static_cast<uint8_t>((i * 5) & 1);
  auto cw = nr_bb::ldpc_encode(info, p);
  nr_bb::RateMatchConfig rm{.e_bits = 80, .rv = 0, .zc = p.zc, .bg = p.bg};
  auto e0 = nr_bb::rate_match(cw, rm);
  rm.rv = 2;
  auto e2 = nr_bb::rate_match(cw, rm);
  REQUIRE(e0.size() == 80);
  REQUIRE(e2.size() == 80);

  nr_bb::SoftVec llr0(e0.size()), llr2(e2.size());
  for (size_t i = 0; i < e0.size(); ++i) {
    llr0[i] = e0[i] ? -8.0 : 8.0;
    llr2[i] = e2[i] ? -8.0 : 8.0;
  }
  auto d0 = nr_bb::rate_dematch(llr0, rm, cw.size());
  rm.rv = 0;
  // re-dematch RV0 into buffer then combine
  rm.rv = 0;
  auto soft0 = nr_bb::rate_dematch(llr0, rm, cw.size());
  rm.rv = 2;
  auto soft2 = nr_bb::rate_dematch(llr2, rm, cw.size());
  nr_bb::HarqBuffer harq;
  harq.upsert(1, 0, soft0, 0);
  harq.upsert(1, 0, soft2, 2);
  auto comb = harq.get(1);
  REQUIRE(comb.has_value());
  REQUIRE(comb->size() == cw.size());
  (void)d0;
}

TEST_CASE("segmentation attaches CRC24B when C>1", "[segmentation][standards]") {
  nr_bb::BitVec tb(9000, 1);
  nr_bb::SegmentationConfig cfg;
  cfg.max_cb_info_bits = 4000;
  cfg.attach_crc24b_when_segmented = true;
  cfg.target_rate = 0.5;
  auto seg = nr_bb::segment_transport_block(tb, cfg);
  REQUIRE(seg.num_code_blocks > 1);
  REQUIRE(seg.code_blocks[0].bits_with_crc.size() >= 24);
  REQUIRE(seg.total_filler_bits >= 0);
  REQUIRE(seg.code_blocks[0].zc > 0);
}
