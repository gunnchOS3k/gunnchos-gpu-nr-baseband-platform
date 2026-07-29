#pragma once
/**
 * EDUCATIONAL_ONLY — short (16,8) regular LDPC. NOT NR BG1/BG2.
 * Do not use as Gate 4 acceptance criteria.
 */
#include "nr_bb/types.hpp"
#include <string>

namespace nr_bb_edu {

struct ShortLdpcParams {
  int n = 16;
  int k = 8;
  int max_iter = 20;
};

nr_bb::BitVec ldpc_encode(const nr_bb::BitVec& info, const ShortLdpcParams& p = {});
nr_bb::BitVec ldpc_decode(const nr_bb::SoftVec& llr, const ShortLdpcParams& p = {});

struct ShortLdpcReference {
  nr_bb::BitVec info;
  nr_bb::BitVec codeword;
  std::string provenance = "self-generated EDUCATIONAL_ONLY (16,8)";
};

ShortLdpcReference make_reference(const nr_bb::BitVec& info, const ShortLdpcParams& p = {});

}  // namespace nr_bb_edu
