#include "nr_bb/crc.hpp"

namespace nr_bb {

namespace {
struct CrcSpec {
  uint32_t poly_full;  // includes leading 1 (degree+1 bits)
  int len;
};

CrcSpec spec(CrcType t) {
  switch (t) {
    case CrcType::CRC24A: return {0x1864CFBu, 24};  // TS 38.212 Table 5.1-1
    case CrcType::CRC24B: return {0x1800063u, 24};
    case CrcType::CRC16:  return {0x11021u, 16};
    case CrcType::CRC11:  return {0xE21u, 11};
    case CrcType::CRC6:   return {0x61u, 6};
  }
  throw Error("unknown CRC type");
}

/** Bit-serial MSB-first CRC; final register is the remainder/CRC bits. */
uint32_t compute_crc(const BitVec& bits, CrcType type) {
  const auto s = spec(type);
  const uint32_t mask = (s.len == 32) ? 0xFFFFFFFFu : ((1u << s.len) - 1u);
  const uint32_t top = 1u << (s.len - 1);
  const uint32_t poly = s.poly_full & mask;
  uint32_t crc = 0;
  for (uint8_t b : bits) {
    require(b == 0 || b == 1, "CRC input must be 0/1 bits");
    const uint32_t fb = ((crc & top) ? 1u : 0u) ^ static_cast<uint32_t>(b);
    crc = (crc << 1) & mask;
    if (fb) crc ^= poly;
  }
  return crc;
}
}  // namespace

uint32_t crc_polynomial(CrcType type) { return spec(type).poly_full; }
int crc_length_bits(CrcType type) { return spec(type).len; }

BitVec crc_attach(const BitVec& payload, CrcType type) {
  const auto s = spec(type);
  const uint32_t rem = compute_crc(payload, type);
  BitVec out = payload;
  out.reserve(payload.size() + static_cast<size_t>(s.len));
  for (int i = s.len - 1; i >= 0; --i) {
    out.push_back(static_cast<uint8_t>((rem >> i) & 1u));
  }
  return out;
}

bool crc_check(const BitVec& coded, CrcType type) {
  const auto s = spec(type);
  require(coded.size() >= static_cast<size_t>(s.len), "coded block shorter than CRC");
  return compute_crc(coded, type) == 0;
}

}  // namespace nr_bb
