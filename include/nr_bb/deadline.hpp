#pragma once
/**
 * @file deadline.hpp
 * @brief Numerology-aware slot deadline model (CPU synthetic timings OK if labeled).
 *
 * NEVER claim GPU timing closure from this model.
 */
#include <cstddef>
#include <string>
#include <vector>

namespace nr_bb {

struct NumerologyConfig {
  int mu = 0;                 // 0..4 → 15*2^μ kHz
  double slot_duration_us = 0;  // if 0, derived from mu
};

struct DeadlineReport {
  int mu = 0;
  double slot_duration_us = 1000.0;
  double deadline_us = 1000.0;
  double median_us = 0;
  double p90_us = 0;
  double p95_us = 0;
  double p99_us = 0;
  double max_us = 0;
  int samples = 0;
  int deadline_misses = 0;
  double miss_ratio = 0;
  std::string timing_source = "cpu_synthetic";
  std::string gpu_claim = "none — NEVER claim GPU timing closure from CPU synthetic";
};

double slot_duration_us_for_mu(int mu);
DeadlineReport evaluate_deadlines(const std::vector<double>& sample_us, const NumerologyConfig& cfg,
                                  double deadline_fraction = 1.0);

}  // namespace nr_bb
