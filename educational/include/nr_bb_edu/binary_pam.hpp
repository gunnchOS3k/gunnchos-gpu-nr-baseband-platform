#pragma once
/** EDUCATIONAL_ONLY — binary PAM (non-Gray). Acceptance uses Gray QAM. */
#include "nr_bb/types.hpp"

namespace nr_bb_edu {

nr_bb::ComplexVec modulate_binary_pam(const nr_bb::BitVec& bits, nr_bb::ModulationOrder order);
/** Constant-magnitude hard LLR (NOT soft max-log-MAP). */
nr_bb::SoftVec hard_constant_llr(const nr_bb::ComplexVec& syms, nr_bb::ModulationOrder order,
                                 double magnitude);

}  // namespace nr_bb_edu
