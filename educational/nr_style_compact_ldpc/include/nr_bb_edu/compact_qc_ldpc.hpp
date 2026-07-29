#pragma once
/**
 * @file compact_qc_ldpc.hpp
 * @brief EDUCATIONAL_ONLY compact QC-LDPC scaffolds (NOT NR acceptance).
 *
 * Former standards-path placeholder. Relocated under educational/ so Gate 4 /
 * Track C acceptance uses real BG1/BG2 tables instead.
 */
#include "nr_bb/types.hpp"
#include <string>
#include <vector>

namespace nr_bb_edu {

enum class CompactBaseGraph { BG1, BG2 };

struct CompactQcLdpcParams {
  CompactBaseGraph bg = CompactBaseGraph::BG2;
  int zc = 2;
  int max_iter = 25;
  int info_cols = -1;
};

struct CompactQcLdpcGraphInfo {
  int mb = 0;
  int nb = 0;
  int kb = 0;
  int zc = 0;
  std::string provenance;
};

CompactQcLdpcGraphInfo compact_qc_ldpc_graph_info(const CompactQcLdpcParams& p);
nr_bb::BitVec compact_qc_ldpc_encode(const nr_bb::BitVec& info, const CompactQcLdpcParams& p = {});
nr_bb::BitVec compact_qc_ldpc_decode(const nr_bb::SoftVec& llr, const CompactQcLdpcParams& p = {});
bool compact_qc_ldpc_syndrome_ok(const nr_bb::BitVec& codeword, const CompactQcLdpcParams& p = {});

}  // namespace nr_bb_edu
