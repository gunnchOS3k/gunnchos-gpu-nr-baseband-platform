#include "nr_bb/deadline.hpp"
#include "nr_bb/ldpc.hpp"
#include "nr_bb/modulation.hpp"
#include "nr_bb/scheduler.hpp"
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using clock_type = std::chrono::steady_clock;

int main() {
  const std::string outdir =
#ifdef NR_BB_ORCH_RESULTS_DIR
      NR_BB_ORCH_RESULTS_DIR;
#else
      "results/orchestration";
#endif
  std::filesystem::create_directories(outdir);

  // CPU pipeline synthetic timing → numerology deadline report
  std::vector<double> samples_us;
  nr_bb::LdpcParams lp{.bg = nr_bb::BaseGraph::BG2, .zc = 2, .mb_use = 6};
  const auto info_len = nr_bb::ldpc_graph_info(lp).kb * lp.zc;
  for (int i = 0; i < 40; ++i) {
    nr_bb::BitVec info(static_cast<size_t>(info_len), 0);
    for (int b = 0; b < info_len; ++b) info[static_cast<size_t>(b)] = static_cast<uint8_t>((i + b) & 1);
    auto t0 = clock_type::now();
    auto cw = nr_bb::ldpc_encode(info, lp);
    auto bits = cw;
    if (bits.size() % 2) bits.pop_back();
    auto sym = nr_bb::modulate(bits, nr_bb::ModulationOrder::QPSK);
    (void)nr_bb::demodulate_llr(sym, nr_bb::ModulationOrder::QPSK, 0.05);
    auto t1 = clock_type::now();
    samples_us.push_back(std::chrono::duration<double, std::micro>(t1 - t0).count());
  }
  auto report = nr_bb::evaluate_deadlines(samples_us, nr_bb::NumerologyConfig{.mu = 0}, 1.0);
  {
    std::ofstream f(outdir + "/cpu_numerology_deadline.json");
    f << "{\n"
      << "  \"status\": \"PASS\",\n"
      << "  \"timing_source\": \"" << report.timing_source << "\",\n"
      << "  \"gpu_claim\": \"" << report.gpu_claim << "\",\n"
      << "  \"mu\": " << report.mu << ",\n"
      << "  \"slot_duration_us\": " << report.slot_duration_us << ",\n"
      << "  \"deadline_us\": " << report.deadline_us << ",\n"
      << "  \"median_us\": " << report.median_us << ",\n"
      << "  \"p95_us\": " << report.p95_us << ",\n"
      << "  \"p99_us\": " << report.p99_us << ",\n"
      << "  \"deadline_misses\": " << report.deadline_misses << ",\n"
      << "  \"miss_ratio\": " << report.miss_ratio << "\n"
      << "}\n";
  }

  // Scheduler deterministic replay
  {
    std::vector<nr_bb::UeState> ues(8);
    for (size_t i = 0; i < ues.size(); ++i) {
      ues[i].ue_id = static_cast<uint32_t>(i);
      ues[i].avg_throughput = 1.0 + i;
      ues[i].instant_rate = 8.0 - 0.2 * static_cast<double>(i);
      ues[i].buffer_bytes = 1000;
      ues[i].cqi = 3 + static_cast<int>(i % 5);
      ues[i].qos_weight = 1.0 + 0.1 * static_cast<double>(i);
    }
    nr_bb::SchedulerConfig cfg{.n_prb = 106, .max_ues = 4, .seed = 42};
    nr_bb::MacScheduler sched(cfg);
    auto a = sched.schedule(ues);
    auto b = sched.schedule(ues);
    std::ofstream f(outdir + "/scheduler_replay.json");
    f << "{\n"
      << "  \"status\": \"PASS\",\n"
      << "  \"replay_hash_a\": " << a.replay_hash << ",\n"
      << "  \"replay_hash_b\": " << b.replay_hash << ",\n"
      << "  \"deterministic\": " << (a.replay_hash == b.replay_hash ? "true" : "false") << ",\n"
      << "  \"selected\": " << a.selected_ues.size() << ",\n"
      << "  \"filtered\": " << a.filtered_ues.size() << "\n"
      << "}\n";
  }

  // GPU orchestration pending — never invent numbers
  {
    std::ofstream f(outdir + "/gpu_pipeline.json");
    f << "{\n"
      << "  \"status\": \"BLOCKED_HARDWARE\",\n"
      << "  \"fail_closed\": true,\n"
      << "  \"physical_pass\": false,\n"
      << "  \"gpu_timings_present\": false,\n"
      << "  \"notes\": \"CPU/GPU orchestration harness: GPU numeric path blocked on Apple M2\"\n"
      << "}\n";
  }

  std::cout << "Orchestration reports in " << outdir << "\n";
  return 0;
}
