#include <catch2/catch_test_macros.hpp>
#include "nr_bb/scheduler.hpp"

TEST_CASE("PF scheduler selects and allocates PRBs", "[sched]") {
  std::vector<nr_bb::UeState> ues = {
      {.ue_id = 1, .avg_throughput = 1.0, .instant_rate = 10.0, .qos_weight = 1.0, .preferred_mcs = 5, .preferred_layers = 2},
      {.ue_id = 2, .avg_throughput = 5.0, .instant_rate = 5.0, .qos_weight = 1.0, .preferred_mcs = 3, .preferred_layers = 1},
      {.ue_id = 3, .avg_throughput = 1.0, .instant_rate = 2.0, .qos_weight = 2.0, .preferred_mcs = 4, .preferred_layers = 1},
  };
  nr_bb::PfScheduler sched(50);
  auto r = sched.schedule(ues, 2);
  REQUIRE(r.selected_ues.size() == 2);
  int prbs = 0;
  for (auto& a : r.allocations) prbs += a.prb_count;
  REQUIRE(prbs == 50);
  REQUIRE(nr_bb::PfScheduler::pf_metric(ues[0]) > nr_bb::PfScheduler::pf_metric(ues[1]));
}
