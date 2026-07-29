#pragma once
/**
 * @file ldpc.hpp
 * @brief Simplified/educational LDPC encode/decode for short blocks.
 *
 * TRACEABILITY NOTE: NR uses QC-LDPC (TS 38.212 §5.3.2, base graphs BG1/BG2).
 * This module implements a small regular LDPC (n=16,k=8) for unit-testable
 * encode/decode and a reference-vector interface. It is NOT NR BG1/BG2
 * conforming and must not be cited as 3GPP LDPC conformance.
 */
#include "nr_bb/types.hpp"
#include <string>
#include <vector>

namespace nr_bb {

struct LdpcParams {
  int n = 16;  // codeword bits
  int k = 8;   // information bits
  int max_iter = 20;
};

/** Systematic encode: returns n bits (info + parity). Input must be k bits. */
BitVec ldpc_encode(const BitVec& info, const LdpcParams& p = {});

/** Soft decode from LLRs (length n); returns k hard bits. */
BitVec ldpc_decode(const SoftVec& llr, const LdpcParams& p = {});

/** Load/store reference vectors (JSON-ish line format handled in tests). */
struct LdpcReferenceVector {
  BitVec info;
  BitVec codeword;
  std::string provenance = "self-generated";
};

LdpcReferenceVector ldpc_make_reference(const BitVec& info, const LdpcParams& p = {});

}  // namespace nr_bb
