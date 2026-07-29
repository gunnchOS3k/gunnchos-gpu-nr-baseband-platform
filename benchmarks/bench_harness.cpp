#include "nr_bb/crc.hpp"
#include "nr_bb/ldpc.hpp"
#include "nr_bb/modulation.hpp"
#include "nr_bb/equalizer.hpp"
#include "nr_bb/scheduler.hpp"
#include "nr_bb/ofdm.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

using clock_type = std::chrono::steady_clock;

struct Stats {
  double median = 0, p90 = 0, p95 = 0, p99 = 0, max = 0, jitter = 0;
  double throughput = 0;
  int deadline_misses = 0;
};

Stats summarize(std::vector<double> us, double work_units, double deadline_us) {
  std::sort(us.begin(), us.end());
  auto pct = [&](double p) {
    if (us.empty()) return 0.0;
    const double idx = p * static_cast<double>(us.size() - 1);
    const size_t i = static_cast<size_t>(idx);
    return us[i];
  };
  Stats s;
  s.median = pct(0.50);
  s.p90 = pct(0.90);
  s.p95 = pct(0.95);
  s.p99 = pct(0.99);
  s.max = us.empty() ? 0.0 : us.back();
  double mean = std::accumulate(us.begin(), us.end(), 0.0) / std::max<size_t>(1, us.size());
  double var = 0;
  for (double v : us) var += (v - mean) * (v - mean);
  var /= std::max<size_t>(1, us.size());
  s.jitter = std::sqrt(var);
  const double total_s = std::accumulate(us.begin(), us.end(), 0.0) * 1e-6;
  s.throughput = total_s > 0 ? work_units / total_s : 0.0;
  for (double v : us) if (v > deadline_us) ++s.deadline_misses;
  return s;
}

void write_json(const std::string& path, const std::string& name, const Stats& s,
                const std::string& status, const std::string& notes) {
  std::filesystem::create_directories(std::filesystem::path(path).parent_path());
  std::ofstream f(path);
  f << "{\n"
    << "  \"benchmark\": \"" << name << "\",\n"
    << "  \"status\": \"" << status << "\",\n"
    << "  \"platform\": \"cpu\",\n"
    << "  \"median_us\": " << s.median << ",\n"
    << "  \"p90_us\": " << s.p90 << ",\n"
    << "  \"p95_us\": " << s.p95 << ",\n"
    << "  \"p99_us\": " << s.p99 << ",\n"
    << "  \"max_us\": " << s.max << ",\n"
    << "  \"jitter_us\": " << s.jitter << ",\n"
    << "  \"throughput\": " << s.throughput << ",\n"
    << "  \"deadline_misses\": " << s.deadline_misses << ",\n"
    << "  \"notes\": \"" << notes << "\"\n"
    << "}\n";
}

template <typename Fn>
Stats run_bench(Fn&& fn, int iters, double work_units, double deadline_us) {
  std::vector<double> samples;
  samples.reserve(static_cast<size_t>(iters));
  for (int i = 0; i < iters; ++i) {
    auto t0 = clock_type::now();
    fn();
    auto t1 = clock_type::now();
    samples.push_back(std::chrono::duration<double, std::micro>(t1 - t0).count());
  }
  return summarize(std::move(samples), work_units, deadline_us);
}

int main() {
  const std::string outdir =
#ifdef NR_BB_RESULTS_DIR
      NR_BB_RESULTS_DIR;
#else
      "results/benchmarks";
#endif
  std::filesystem::create_directories(outdir);

  // Pageable vs pinned mock: vector vs aligned buffer concepts on CPU
  {
    std::vector<float> pageable(1 << 20, 1.0f);
    alignas(64) float pinned_stack[4096];
    for (auto& v : pinned_stack) v = 1.0f;
    auto st = run_bench([&] {
      volatile float acc = 0;
      for (size_t i = 0; i < pageable.size(); i += 64) acc += pageable[i];
      for (int i = 0; i < 4096; i += 16) acc += pinned_stack[i];
      (void)acc;
    }, 200, 1e6, 5000.0);
    write_json(outdir + "/cpu_memory_layout.json", "cpu_memory_layout_pageable_pinned_mock", st, "PASS",
               "CPU mock of pageable/pinned concepts; not CUDA pinned host memory");
  }

  {
    nr_bb::BitVec bits(256);
    for (size_t i = 0; i < bits.size(); ++i) bits[i] = static_cast<uint8_t>(i & 1);
    auto st = run_bench([&] {
      auto c = nr_bb::crc_attach(bits, nr_bb::CrcType::CRC24A);
      (void)nr_bb::crc_check(c, nr_bb::CrcType::CRC24A);
    }, 500, 256.0 * 500, 200.0);
    write_json(outdir + "/cpu_crc24a.json", "cpu_crc24a", st, "PASS", "educational CRC24A");
  }

  {
    nr_bb::BitVec info = {1, 0, 1, 1, 0, 0, 1, 0};
    auto st = run_bench([&] {
      auto cw = nr_bb::ldpc_encode(info);
      nr_bb::SoftVec llr(16);
      for (size_t i = 0; i < 16; ++i) llr[i] = cw[i] ? -5.0 : 5.0;
      (void)nr_bb::ldpc_decode(llr);
    }, 300, 300.0, 500.0);
    write_json(outdir + "/cpu_ldpc_short.json", "cpu_ldpc_short", st, "PASS", "educational (16,8) LDPC");
  }

  {
    nr_bb::OfdmConfig cfg{.fft_size = 64, .cp_len = 8};
    nr_bb::ComplexVec freq(64, {0.1, -0.2});
    auto st = run_bench([&] {
      auto t = nr_bb::ofdm_modulate(freq, cfg);
      (void)nr_bb::ofdm_demodulate(t, cfg);
    }, 100, 100.0, 2000.0);
    write_json(outdir + "/cpu_ofdm_64.json", "cpu_ofdm_64", st, "PASS", "naive DFT OFDM");
  }

  {
    std::vector<nr_bb::Complex> H(16, {0, 0});
    for (int i = 0; i < 4; ++i) H[static_cast<size_t>(i * 4 + i)] = {1, 0};
    nr_bb::ComplexVec y(4, {0.2, -0.1});
    nr_bb::EqualizerConfig cfg;
    cfg.type = nr_bb::EqualizerType::MMSE;
    cfg.noise_var = 0.01;
    auto st = run_bench([&] { (void)nr_bb::equalize_sample(y, H, 4, 4, cfg); }, 1000, 1000.0, 50.0);
    write_json(outdir + "/cpu_eq_mmse_4x4.json", "cpu_eq_mmse_4x4", st, "PASS", "4x4 MMSE");
  }

  {
    std::vector<nr_bb::UeState> ues(16);
    for (size_t i = 0; i < ues.size(); ++i) {
      ues[i].ue_id = static_cast<uint32_t>(i);
      ues[i].avg_throughput = 1.0 + static_cast<double>(i);
      ues[i].instant_rate = 10.0 - static_cast<double>(i) * 0.1;
      ues[i].preferred_mcs = 4;
      ues[i].preferred_layers = 1 + static_cast<int>(i % 2);
    }
    nr_bb::PfScheduler sched(273);
    auto st = run_bench([&] { (void)sched.schedule(ues, 8); }, 2000, 2000.0, 20.0);
    write_json(outdir + "/cpu_scheduler_pf.json", "cpu_scheduler_pf", st, "PASS", "PF CPU baseline");
  }

  // CUDA graphs/streams slot — blocked on this host
  {
    Stats s{};
    write_json(outdir + "/cuda_graphs_streams.json", "cuda_graphs_streams", s, "BLOCKED_HARDWARE",
               "No NVIDIA GPU/CUDA toolkit on Apple M2 host");
  }

  std::cout << "Benchmarks written to " << outdir << "\n";
  return 0;
}
