#include <catch2/catch_test_macros.hpp>
#include "nr_bb/crc.hpp"

TEST_CASE("CRC24A attach and check", "[crc]") {
  nr_bb::BitVec bits = {1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1};
  auto coded = nr_bb::crc_attach(bits, nr_bb::CrcType::CRC24A);
  REQUIRE(coded.size() == bits.size() + 24);
  REQUIRE(nr_bb::crc_check(coded, nr_bb::CrcType::CRC24A));
  coded.back() = static_cast<uint8_t>(1 - coded.back());
  REQUIRE_FALSE(nr_bb::crc_check(coded, nr_bb::CrcType::CRC24A));
}

TEST_CASE("CRC polynomials documented", "[crc]") {
  REQUIRE(nr_bb::crc_polynomial(nr_bb::CrcType::CRC24A) == 0x1864CFBu);
  REQUIRE(nr_bb::crc_polynomial(nr_bb::CrcType::CRC24B) == 0x1800063u);
  REQUIRE(nr_bb::crc_length_bits(nr_bb::CrcType::CRC24A) == 24);
}

TEST_CASE("CRC error handling", "[crc]") {
  auto coded = nr_bb::crc_attach({}, nr_bb::CrcType::CRC16);
  REQUIRE(coded.size() == 16);
  REQUIRE_THROWS_AS(nr_bb::crc_check(nr_bb::BitVec{1, 0, 1}, nr_bb::CrcType::CRC24A), nr_bb::Error);
}
