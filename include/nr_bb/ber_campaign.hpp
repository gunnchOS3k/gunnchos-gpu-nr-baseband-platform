#pragma once
/**
 * @file ber_campaign.hpp
 * @brief BER/BLER campaign hooks over modulate → AWGN → soft demod → hard compare.
 */
#include "nr_bb/types.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace nr_bb {

struct BerPoint {
  double snr_db = 0;
  double ber = 0;
  double bler = 0;
  std::uint64_t bits = 0;
  std::uint64_t bit_errors = 0;
  std::uint64_t blocks = 0;
  std::uint64_t block_errors = 0;
};

struct BerCampaignResult {
  ModulationOrder order = ModulationOrder::QPSK;
  std::vector<BerPoint> points;
  std::string provenance = "cpu_awgn_self_campaign";
};

BerCampaignResult run_ber_campaign(ModulationOrder order, const std::vector<double>& snr_db_list,
                                   std::size_t bits_per_snr, uint32_t seed = 1);

}  // namespace nr_bb
