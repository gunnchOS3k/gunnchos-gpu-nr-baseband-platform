#pragma once
/**
 * @file crc.hpp
 * @brief CRC24A/CRC24B style educational CRC (poly from 3GPP TS 38.212 Table 5.1-1).
 *
 * TRACEABILITY NOTE: Polynomials and remainder width follow TS 38.212 §5.1.
 * Implementation is educational/bit-serial and is NOT a full conformance claim.
 */
#include "nr_bb/types.hpp"
#include <cstdint>
#include <vector>

namespace nr_bb {

enum class CrcType { CRC24A, CRC24B, CRC16, CRC11, CRC6 };

/** Compute CRC remainder bits (MSB-first bit packing into BitVec). */
BitVec crc_attach(const BitVec& payload, CrcType type);

/** Verify CRC; returns true if remainder is zero after check. */
bool crc_check(const BitVec& coded, CrcType type);

/** Polynomial constants (hex) for documentation / golden vectors. */
uint32_t crc_polynomial(CrcType type);
int crc_length_bits(CrcType type);

}  // namespace nr_bb
