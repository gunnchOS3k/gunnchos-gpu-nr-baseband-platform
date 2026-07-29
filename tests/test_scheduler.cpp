#include <catch2/catch_test_macros.hpp>
#include "nr_bb/scheduler.hpp"

TEST_CASE("MacScheduler filters and allocates deterministically", "[scheduler]") {
  std::vector<nr_bb::UeState> ues(5);
  for (size_t i = 0; i < ues.size(); ++i) {
    ues[i].ue_id = static_cast<uint32_t>(i);
    ues[i].avg_throughput = 1.0 + i;
    ues[i].instant_rate = 5.0;
    ues[i].buffer_bytes = (i == 0) ? 0.0 : 1000.0;  // filter UE0
    ues[i].cqi = 5;
  }
  nr_bb::MacScheduler sched(nr_bb::SchedulerConfig{.n_prb = 50, .max_ues = 3, .seed = 9});
  auto a = sched.schedule(ues);
  auto b = sched.schedule(ues);
  REQUIRE(a.replay_hash == b.replay_hash);
  REQUIRE(!a.filtered_ues.empty());
  REQUIRE(a.selected_ues.size() <= 3);
  int prb = 0;
  for (const auto& x : a.allocations) prb += x.prb_count;
  REQUIRE(prb == 50);
}

TEST_CASE("PfScheduler compatibility", "[scheduler]") {
  std::vector<nr_bb::UeState> ues(4);
  for (size_t i = 0; i < ues.size(); ++i) {
    ues[i].ue_id = static_cast<uint32_t>(i);
    ues[i].buffer_bytes = 10;
  }
  nr_bb::PfScheduler pf(100);
  auto r = pf.schedule(ues, 2);
  REQUIRE(r.selected_ues.size() == 2);
}
