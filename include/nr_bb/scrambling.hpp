#pragma once
/**
 * @file scrambling.hpp
 * @brief Gold-sequence style scrambling (educational).
 *
 * TRACEABILITY NOTE: NR scrambling uses Gold sequences (TS 38.211 §5.2.1,
 * §6.3.1.1 for PUSCH). This implements an x1/x2 LFSR pair with c_init seeding.
 * Educational; NOT a full multi-sequence NR conformance claim.
 */
#include "nr_bb/types.hpp"
#include <cstdint>

namespace nr_bb {

BitVec gold_sequence(std::size_t length, uint32_t c_init);
BitVec scramble(const BitVec& bits, uint32_t c_init);
BitVec descramble(const BitVec& bits, uint32_t c_init);  // same as scramble (XOR)

}  // namespace nr_bb
