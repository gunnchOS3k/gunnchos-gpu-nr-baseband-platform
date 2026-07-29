#include "nr_bb/scheduler.hpp"
#include <algorithm>
#include <cmath>

namespace nr_bb {

double MacScheduler::pf_metric(const UeState& ue) {
  const double denom = std::max(ue.avg_throughput, 1e-9);
  return ue.qos_weight * (ue.instant_rate / denom);
}

double MacScheduler::composite_metric(const UeState& ue, bool enable_qos) {
  double m = pf_metric(ue);
  m *= (1.0 + 0.05 * static_cast<double>(ue.cqi));
  m *= (1.0 + 0.02 * static_cast<double>(ue.preferred_layers));
  if (enable_qos) m *= std::max(0.1, ue.qos_weight);
  // Prefer UEs with pending buffer and available HARQ budget
  m *= (1.0 + std::min(ue.buffer_bytes, 1e6) / 1e6);
  m *= (1.0 + 0.1 * static_cast<double>(std::max(0, ue.harq_processes_max - ue.harq_processes_active)));
  return m;
}

namespace {
uint64_t mix64(uint64_t x) {
  x ^= x >> 30;
  x *= 0xbf58476d1ce4e5b9ULL;
  x ^= x >> 27;
  x *= 0x94d049bb133111ebULL;
  x ^= x >> 31;
  return x;
}
}  // namespace

ScheduleResult MacScheduler::schedule(const std::vector<UeState>& ues) const {
  require(cfg_.n_prb > 0, "n_prb must be > 0");
  require(cfg_.max_ues > 0, "max_ues must be > 0");
  ScheduleResult out;
  std::vector<UeState> eligible;
  eligible.reserve(ues.size());
  for (const auto& ue : ues) {
    const bool pass = ue.rrc_connected && !ue.filtered_out && ue.buffer_bytes >= cfg_.min_buffer_bytes &&
                      ue.harq_processes_active < cfg_.max_harq_active &&
                      ue.harq_processes_active < ue.harq_processes_max;
    if (!pass) {
      out.filtered_ues.push_back(ue.ue_id);
      continue;
    }
    eligible.push_back(ue);
  }
  std::stable_sort(eligible.begin(), eligible.end(), [&](const UeState& a, const UeState& b) {
    return composite_metric(a, cfg_.enable_qos) > composite_metric(b, cfg_.enable_qos);
  });
  const int n_sel = std::min(cfg_.max_ues, static_cast<int>(eligible.size()));
  if (n_sel == 0) {
    out.replay_hash = mix64(cfg_.seed ^ static_cast<uint64_t>(cfg_.n_prb));
    return out;
  }
  // Weighted PRB packing by metric share
  double sum_m = 0;
  for (int i = 0; i < n_sel; ++i) sum_m += std::max(1e-9, composite_metric(eligible[static_cast<size_t>(i)], cfg_.enable_qos));
  int cursor = 0;
  int remaining = cfg_.n_prb;
  for (int i = 0; i < n_sel; ++i) {
    const auto& ue = eligible[static_cast<size_t>(i)];
    const double m = composite_metric(ue, cfg_.enable_qos);
    int n = (i == n_sel - 1) ? remaining
                             : std::max(1, static_cast<int>(std::floor(cfg_.n_prb * (m / sum_m))));
    if (n > remaining) n = remaining;
    // MCS from CQI with preferred hint
    int mcs = ue.preferred_mcs;
    if (ue.cqi >= 10) mcs = std::max(mcs, 16);
    else if (ue.cqi >= 7) mcs = std::max(mcs, 10);
    else if (ue.cqi <= 2) mcs = std::min(mcs, 2);
    PrbAllocation a;
    a.ue_id = ue.ue_id;
    a.prb_start = cursor;
    a.prb_count = n;
    a.mcs = mcs;
    a.layers = ue.preferred_layers;
    a.metric = m;
    cursor += n;
    remaining -= n;
    out.allocations.push_back(a);
    out.selected_ues.push_back(ue.ue_id);
  }
  uint64_t h = cfg_.seed;
  for (const auto& a : out.allocations) {
    h = mix64(h ^ (static_cast<uint64_t>(a.ue_id) << 32) ^ static_cast<uint64_t>(a.prb_start) ^
              (static_cast<uint64_t>(a.prb_count) << 16) ^ static_cast<uint64_t>(a.mcs));
  }
  out.replay_hash = h;
  return out;
}

}  // namespace nr_bb
