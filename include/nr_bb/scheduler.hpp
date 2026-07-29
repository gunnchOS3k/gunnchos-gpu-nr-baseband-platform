#pragma once
/**
 * @file scheduler.hpp
 * @brief MAC-style scheduler beyond PF-ratio-only: UE filter, metrics, PRB, MCS, QoS, HARQ, replay.
 *
 * TRACEABILITY NOTE: Research baseline. NOT 3GPP MAC conformance.
 */
#include "nr_bb/types.hpp"
#include <cstdint>
#include <vector>

namespace nr_bb {

struct UeState {
  uint32_t ue_id = 0;
  double avg_throughput = 1.0;
  double instant_rate = 1.0;
  int harq_processes_active = 0;
  int harq_processes_max = 16;
  double qos_weight = 1.0;
  double buffer_bytes = 1000.0;
  bool rrc_connected = true;
  bool filtered_out = false;
  int preferred_mcs = 4;
  int preferred_layers = 1;
  int cqi = 4;
};

struct PrbAllocation {
  uint32_t ue_id = 0;
  int prb_start = 0;
  int prb_count = 0;
  int mcs = 0;
  int layers = 1;
  double metric = 0.0;
};

struct ScheduleResult {
  std::vector<PrbAllocation> allocations;
  std::vector<uint32_t> selected_ues;
  std::vector<uint32_t> filtered_ues;
  uint64_t replay_hash = 0;
};

struct SchedulerConfig {
  int n_prb = 50;
  int max_ues = 4;
  int max_harq_active = 8;
  double min_buffer_bytes = 1.0;
  bool enable_qos = true;
  uint64_t seed = 1;
};

class MacScheduler {
 public:
  explicit MacScheduler(SchedulerConfig cfg = {}) : cfg_(std::move(cfg)) {}
  ScheduleResult schedule(const std::vector<UeState>& ues) const;
  static double composite_metric(const UeState& ue, bool enable_qos);
  static double pf_metric(const UeState& ue);

 private:
  SchedulerConfig cfg_;
};

/** Compatibility wrapper: PF-oriented schedule with MacScheduler backend. */
class PfScheduler {
 public:
  explicit PfScheduler(int n_prb = 50) : n_prb_(n_prb) {}
  ScheduleResult schedule(const std::vector<UeState>& ues, int max_ues = 4) const {
    SchedulerConfig cfg;
    cfg.n_prb = n_prb_;
    cfg.max_ues = max_ues;
    return MacScheduler(cfg).schedule(ues);
  }
  static double pf_metric(const UeState& ue) { return MacScheduler::pf_metric(ue); }

 private:
  int n_prb_;
};

}  // namespace nr_bb
