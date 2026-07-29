#pragma once
/**
 * @file ldpc.hpp
 * @brief Real 5G NR QC-LDPC (BG1/BG2) — lifting, encode, min-sum decode.
 *
 * PROVENANCE / LICENSE
 * - Spec: 3GPP TS 38.212 §5.2.2 / §5.3.2 (BG selection, Zc sets, QC lifting).
 * - Base-graph shift tables: Apache-2.0 Sionna CSVs (see ldpc_bg_tables.hpp /
 *   docs/reference/LDPC_SOURCE_AND_LICENSE_AUDIT.md).
 * - Encode: systematic GF(2) solve on lifted H (honest; CI uses modest Zc).
 * - Decode: layered normalized min-sum BP (public-domain algorithm).
 * - NO proprietary NVIDIA Aerial / cuBB source.
 *
 * Compact scaffolds live under educational/nr_style_compact_ldpc/ and are
 * EDUCATIONAL_ONLY — not acceptance.
 */
#include "nr_bb/types.hpp"
#include <optional>
#include <string>
#include <vector>

namespace nr_bb {

enum class BaseGraph { BG1, BG2 };

struct LdpcParams {
  BaseGraph bg = BaseGraph::BG2;
  int zc = 0;            // 0 => select via select_lifting from info_bits
  int i_ls = -1;         // -1 => derived from zc
  int max_iter = 25;
  int info_bits = -1;    // K' before filler; -1 => full kb*zc
  int kb = -1;           // -1 => BG default (22 / 10)
  int mb_use = -1;       // -1 => full mb; else puncture extension rows
};

struct LdpcGraphInfo {
  int mb = 0;
  int nb = 0;
  int kb = 0;
  int zc = 0;
  int i_ls = 0;
  int n_ldpc = 0;  // nb * zc
  int k_ldpc = 0;  // kb * zc
  int filler_bits = 0;
  std::string provenance;
};

/** TS 38.212 §5.2.2 base-graph selection from payload length K and rate R=K/N. */
BaseGraph select_basegraph(int k_bits, double coderate);

/** Table 5.3.2-1: return (Zc, i_ls, kb_for_selection). Final encode kb is 22/10. */
struct LiftingChoice {
  int zc = 0;
  int i_ls = 0;
  int kb = 0;
};
LiftingChoice select_lifting(int k_bits, BaseGraph bg);

/** True if Zc is in Table 5.3.2-1. */
bool is_valid_lifting_size(int zc);
std::optional<int> i_ls_for_zc(int zc);

LdpcGraphInfo ldpc_graph_info(const LdpcParams& p);

BitVec ldpc_encode(const BitVec& info, const LdpcParams& p = {});

/** Soft decode from channel LLR length N=nb*zc (or punctured length with zeros). */
BitVec ldpc_decode(const SoftVec& llr, const LdpcParams& p = {});

struct LdpcReferenceVector {
  BitVec info;
  BitVec codeword;
  LdpcParams params;
  std::string provenance;
};

LdpcReferenceVector ldpc_make_reference(const BitVec& info, const LdpcParams& p = {});

bool ldpc_syndrome_ok(const BitVec& codeword, const LdpcParams& p = {});

}  // namespace nr_bb
