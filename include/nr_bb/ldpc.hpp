#pragma once
/**
 * @file ldpc.hpp
 * @brief NR-style QC-LDPC BG1/BG2 lifting basics + iterative min-sum decode.
 *
 * PROVENANCE / LICENSE
 * - Algorithm structure: 3GPP TS 38.212 §5.3.2 (QC lifting, BG1/BG2 roles).
 * - Base-graph edge tables shipped here are compact research scaffolds sized for
 *   CI (small Zc), not a verbatim dump of the full 46×68 / 42×52 3GPP tables.
 * - Encode uses GF(2) systematic solve on the lifted H (honest for small Zc).
 * - Decode uses layered min-sum belief propagation (public-domain algorithm).
 * - NO proprietary NVIDIA / Aerial / cuBB source was copied.
 * - Status: standards-path scaffolding — NOT a claim of full TS 38.212 bit-exact
 *   encode across all Zc / all rate-matching configurations.
 */
#include "nr_bb/types.hpp"
#include <string>
#include <vector>

namespace nr_bb {

enum class BaseGraph { BG1, BG2 };

struct LdpcParams {
  BaseGraph bg = BaseGraph::BG2;
  int zc = 2;          // lifting size (power-of-two research set: 2,4,8)
  int max_iter = 25;
  int info_cols = -1;  // -1 => default Kb for BG (BG1:6 compact, BG2:4 compact)
};

struct LdpcGraphInfo {
  int mb = 0;  // check-node blocks
  int nb = 0;  // variable-node blocks
  int kb = 0;  // information blocks
  int zc = 0;
  std::string provenance;
};

/** Lifted codeword length N = nb * zc; info length K = kb * zc. */
LdpcGraphInfo ldpc_graph_info(const LdpcParams& p);

BitVec ldpc_encode(const BitVec& info, const LdpcParams& p = {});

/** Soft decode from channel LLR length N; returns K hard info bits. */
BitVec ldpc_decode(const SoftVec& llr, const LdpcParams& p = {});

struct LdpcReferenceVector {
  BitVec info;
  BitVec codeword;
  LdpcParams params;
  std::string provenance = "self-generated NR QC-LDPC scaffold";
};

LdpcReferenceVector ldpc_make_reference(const BitVec& info, const LdpcParams& p = {});

/** Syndrome check H*c == 0 over GF(2). */
bool ldpc_syndrome_ok(const BitVec& codeword, const LdpcParams& p = {});

}  // namespace nr_bb
