#include "nr_bb/scheduler.hpp"
#include <algorithm>
#include <numeric>

namespace nr_bb {

double PfScheduler::pf_metric(const UeState& ue) {
  const double denom = std::max(ue.avg_throughput, 1e-9);
  return ue.qos_weight * (ue.instant_rate / denom);
}

ScheduleResult PfScheduler::schedule(const std::vector<UeState>& ues, int max_ues) {
  require(n_prb_ > 0, "n_prb must be > 0");
  require(max_ues > 0, "max_ues must be > 0");
  std::vector<UeState> ranked = ues;
  std::stable_sort(ranked.begin(), ranked.end(), [](const UeState& a, const UeState& b) {
    return pf_metric(a) > pf_metric(b);
  });
  ScheduleResult out;
  const int n_sel = std::min(max_ues, static_cast<int>(ranked.size()));
  if (n_sel == 0) return out;
  const int base = n_prb_ / n_sel;
  int rem = n_prb_ % n_sel;
  int cursor = 0;
  for (int i = 0; i < n_sel; ++i) {
    const auto& ue = ranked[static_cast<size_t>(i)];
    out.selected_ues.push_back(ue.ue_id);
    PrbAllocation a;
    a.ue_id = ue.ue_id;
    a.prb_start = cursor;
    a.prb_count = base + (rem > 0 ? 1 : 0);
    if (rem > 0) --rem;
    a.mcs = ue.preferred_mcs;
    a.layers = ue.preferred_layers;
    cursor += a.prb_count;
    out.allocations.push_back(a);
  }
  return out;
}

}  // namespace nr_bb
