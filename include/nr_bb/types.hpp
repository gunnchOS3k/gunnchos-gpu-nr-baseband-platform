#pragma once
/**
 * @file types.hpp
 * @brief Shared numeric and error types for the NR baseband vertical slice.
 *
 * TRACEABILITY NOTE (educational): Types align conceptually with 3GPP TS 38.211
 * complex baseband samples and TS 38.212 bit sequences. This is NOT a claim of
 * 3GPP conformance or carrier-grade status.
 */
#include <complex>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace nr_bb {

using Complex = std::complex<double>;
using ComplexVec = std::vector<Complex>;
using BitVec = std::vector<uint8_t>;  // 0/1 bits
using SoftVec = std::vector<double>;  // LLRs

enum class ModulationOrder : int {
  QPSK = 2,
  QAM16 = 4,
  QAM64 = 6,
  QAM256 = 8
};

enum class EqualizerType { ZF, MMSE, RZF };

class Error : public std::runtime_error {
 public:
  explicit Error(const std::string& msg) : std::runtime_error(msg) {}
};

inline void require(bool cond, const std::string& msg) {
  if (!cond) throw Error(msg);
}

}  // namespace nr_bb
