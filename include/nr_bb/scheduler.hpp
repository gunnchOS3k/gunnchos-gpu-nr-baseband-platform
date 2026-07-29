#pragma once
/**
 * @file scheduler.hpp
 * @brief CPU proportional-fair scheduler baseline.
 *
 * TRACEABILITY NOTE: PF metric and PRB allocation are research baselines inspired
 * by common MAC scheduling literature / O-RAN Near-RT RIC discussions. NOT a
 * 3GPP MAC scheduler conformance claim.
 */
#include "nr_bb/types.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace nr_bb {

struct UeState {
  uint32_t ue_id = 0;
  double avg_throughput = 1.0;
  double instant_rate = 1.0;
  int harq_processes_active = 0;
  double qos_weight = 1.0;
  int preferred_mcs = 4;
  int preferred_layers = 1;
};

struct PrbAllocation {
  uint32_t ue_id = 0;
  int prb_start = 0;
  int prb_count = 0;
  int mcs = 0;
  int layers = 1;
};

struct ScheduleResult {
  std::vector<PrbAllocation> allocations;
  std::vector<uint32_t> selected_ues;
};

class PfScheduler {
 public:
  explicit PfScheduler(int n_prb = 50) : n_prb_(n_prb) {}
  ScheduleResult schedule(const std::vector<UeState>& ues, int max_ues = 4);
  static double pf_metric(const UeState& ue);

 private:
  int n_prb_;
};

}  // namespace nr_bb
