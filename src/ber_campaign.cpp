#include "nr_bb/ber_campaign.hpp"
#include "nr_bb/modulation.hpp"
#include <cmath>
#include <random>

namespace nr_bb {

BerCampaignResult run_ber_campaign(ModulationOrder order, const std::vector<double>& snr_db_list,
                                   std::size_t bits_per_snr, uint32_t seed) {
  require(bits_per_snr > 0, "bits_per_snr");
  const int m = bits_per_symbol(order);
  require(bits_per_snr % static_cast<size_t>(m) == 0, "bits multiple of Qm");
  BerCampaignResult out;
  out.order = order;
  std::mt19937 rng(seed);
  std::uniform_int_distribution<int> bit_dist(0, 1);
  std::normal_distribution<double> norm(0.0, 1.0);
  for (double snr_db : snr_db_list) {
    const double snr = std::pow(10.0, snr_db / 10.0);
    const double noise_var = 1.0 / snr;  // unit symbol energy
    BitVec bits(bits_per_snr);
    for (auto& b : bits) b = static_cast<uint8_t>(bit_dist(rng));
    auto tx = modulate(bits, order);
    ComplexVec rx = tx;
    const double sigma = std::sqrt(noise_var / 2.0);
    for (auto& z : rx) z += Complex{sigma * norm(rng), sigma * norm(rng)};
    auto hat = demodulate_hard(rx, order);
    BerPoint p;
    p.snr_db = snr_db;
    p.bits = bits.size();
    p.blocks = 1;
    for (std::size_t i = 0; i < bits.size(); ++i)
      if (bits[i] != hat[i]) ++p.bit_errors;
    p.ber = static_cast<double>(p.bit_errors) / static_cast<double>(p.bits);
    if (p.bit_errors) ++p.block_errors;
    p.bler = static_cast<double>(p.block_errors);
    out.points.push_back(p);
  }
  return out;
}

}  // namespace nr_bb
