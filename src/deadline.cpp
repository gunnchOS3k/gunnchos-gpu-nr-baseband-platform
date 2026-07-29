#include "nr_bb/deadline.hpp"
#include "nr_bb/types.hpp"
#include <algorithm>
#include <cmath>

namespace nr_bb {

double slot_duration_us_for_mu(int mu) {
  require(mu >= 0 && mu <= 4, "mu must be 0..4");
  // NR slot length = 1 ms / 2^μ
  return 1000.0 / static_cast<double>(1 << mu);
}

DeadlineReport evaluate_deadlines(const std::vector<double>& sample_us, const NumerologyConfig& cfg,
                                  double deadline_fraction) {
  require(deadline_fraction > 0 && deadline_fraction <= 1.0, "deadline_fraction");
  DeadlineReport r;
  r.mu = cfg.mu;
  r.slot_duration_us = (cfg.slot_duration_us > 0) ? cfg.slot_duration_us : slot_duration_us_for_mu(cfg.mu);
  r.deadline_us = r.slot_duration_us * deadline_fraction;
  r.samples = static_cast<int>(sample_us.size());
  if (sample_us.empty()) return r;
  auto us = sample_us;
  std::sort(us.begin(), us.end());
  auto pct = [&](double p) {
    const double idx = p * static_cast<double>(us.size() - 1);
    return us[static_cast<size_t>(idx)];
  };
  r.median_us = pct(0.50);
  r.p90_us = pct(0.90);
  r.p95_us = pct(0.95);
  r.p99_us = pct(0.99);
  r.max_us = us.back();
  for (double v : us)
    if (v > r.deadline_us) ++r.deadline_misses;
  r.miss_ratio = static_cast<double>(r.deadline_misses) / static_cast<double>(us.size());
  return r;
}

}  // namespace nr_bb
