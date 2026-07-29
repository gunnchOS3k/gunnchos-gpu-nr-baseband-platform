#pragma once
/**
 * @file metrics.hpp
 * @brief BER / BLER metrics.
 *
 * TRACEABILITY NOTE: Standard link-level metrics for research reporting.
 * Not a claim of 3GPP conformance test methodology.
 */
#include "nr_bb/types.hpp"
#include <cstddef>

namespace nr_bb {

struct LinkMetrics {
  std::size_t bit_errors = 0;
  std::size_t bit_total = 0;
  std::size_t block_errors = 0;
  std::size_t block_total = 0;
  double ber() const { return bit_total ? static_cast<double>(bit_errors) / static_cast<double>(bit_total) : 0.0; }
  double bler() const {
    return block_total ? static_cast<double>(block_errors) / static_cast<double>(block_total) : 0.0;
  }
};

void accumulate_ber(LinkMetrics& m, const BitVec& ref, const BitVec& hat);
void accumulate_bler(LinkMetrics& m, bool crc_ok);

}  // namespace nr_bb
