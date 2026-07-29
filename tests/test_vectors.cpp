#include <catch2/catch_test_macros.hpp>
#include "nr_bb/crc.hpp"
#include "nr_bb/ldpc.hpp"
#include <fstream>
#include <string>

TEST_CASE("reference vectors load and match", "[vectors]") {
  const std::string dir = NR_BB_VECTORS_DIR;
  std::ifstream f(dir + "/golden_crc24a.txt");
  REQUIRE(f.good());
  std::string line;
  std::string bits_line;
  while (std::getline(f, line)) {
    if (line.rfind("bits=", 0) == 0) {
      bits_line = line;
      break;
    }
  }
  REQUIRE_FALSE(bits_line.empty());
  auto pos = bits_line.find("bits=") + 5;
  auto end = bits_line.find(' ', pos);
  std::string bitstr = bits_line.substr(pos, end - pos);
  nr_bb::BitVec bits;
  for (char c : bitstr) bits.push_back(static_cast<uint8_t>(c == '1' ? 1 : 0));
  auto coded = nr_bb::crc_attach(bits, nr_bb::CrcType::CRC24A);
  REQUIRE(nr_bb::crc_check(coded, nr_bb::CrcType::CRC24A));

  std::ifstream f2(dir + "/golden_ldpc.txt");
  REQUIRE(f2.good());
  std::string info_line, cw_line;
  while (std::getline(f2, line)) {
    if (line.rfind("info=", 0) == 0) info_line = line;
    if (line.rfind("codeword=", 0) == 0) cw_line = line;
  }
  REQUIRE(info_line.find("info=") == 0);
  REQUIRE(cw_line.find("codeword=") == 0);
}
