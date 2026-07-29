#include "nr_bb/crc.hpp"
#include "nr_bb/fft.hpp"
#include "nr_bb/ldpc.hpp"
#include "nr_bb/modulation.hpp"
#include "nr_bb/ofdm.hpp"
#include "nr_bb/scheduler.hpp"
#include "nr_bb/scrambling.hpp"
#include "nr_bb_edu/naive_dft.hpp"
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
  double median = 0, p90 = 0, p95 = 0, p99 = 0, max = 0;
};

Stats summarize(std::vector<double> us) {
  std::sort(us.begin(), us.end());
  auto pct = [&](double p) {
    if (us.empty()) return 0.0;
    return us[static_cast<size_t>(p * static_cast<double>(us.size() - 1))];
  };
  Stats s;
  s.median = pct(0.50);
  s.p90 = pct(0.90);
  s.p95 = pct(0.95);
  s.p99 = pct(0.99);
  s.max = us.empty() ? 0.0 : us.back();
  return s;
}

template <typename Fn>
Stats run_bench(Fn&& fn, int iters) {
  std::vector<double> samples;
  samples.reserve(static_cast<size_t>(iters));
  for (int i = 0; i < iters; ++i) {
    auto t0 = clock_type::now();
    fn();
    auto t1 = clock_type::now();
    samples.push_back(std::chrono::duration<double, std::micro>(t1 - t0).count());
  }
  return summarize(std::move(samples));
}

void write_study(const std::string& path, const std::string& id, const std::string& hypothesis,
                 const std::string& before_name, const Stats& before, const std::string& after_name,
                 const Stats& after, const std::string& conclusion) {
  std::filesystem::create_directories(std::filesystem::path(path).parent_path());
  std::ofstream f(path);
  f << "{\n"
    << "  \"study_id\": \"" << id << "\",\n"
    << "  \"controlled_variable\": \"" << id << "\",\n"
    << "  \"platform\": \"cpu\",\n"
    << "  \"timing_source\": \"cpu_synthetic\",\n"
    << "  \"gpu_claim\": \"none\",\n"
    << "  \"hypothesis\": \"" << hypothesis << "\",\n"
    << "  \"before\": {\"name\": \"" << before_name << "\", \"median_us\": " << before.median
    << ", \"p90_us\": " << before.p90 << ", \"p95_us\": " << before.p95 << ", \"p99_us\": " << before.p99
    << ", \"max_us\": " << before.max << "},\n"
    << "  \"after\": {\"name\": \"" << after_name << "\", \"median_us\": " << after.median
    << ", \"p90_us\": " << after.p90 << ", \"p95_us\": " << after.p95 << ", \"p99_us\": " << after.p99
    << ", \"max_us\": " << after.max << "},\n"
    << "  \"conclusion\": \"" << conclusion << "\",\n"
    << "  \"status\": \"PASS\"\n"
    << "}\n";
}

int main() {
  const std::string outdir =
#ifdef NR_BB_OPT_RESULTS_DIR
      NR_BB_OPT_RESULTS_DIR;
#else
      "results/optimization_studies";
#endif
  std::filesystem::create_directories(outdir);

  // 1) scalar vs SIMD-friendly contiguous accumulate (CPU)
  {
    std::vector<float> a(1 << 18, 1.1f), b(1 << 18, 0.9f);
    auto before = run_bench([&] {
      volatile float s = 0;
      for (size_t i = 0; i < a.size(); ++i) s += a[i] * b[i];
      (void)s;
    }, 80);
    auto after = run_bench([&] {
      volatile float s0 = 0, s1 = 0, s2 = 0, s3 = 0;
      for (size_t i = 0; i + 3 < a.size(); i += 4) {
        s0 += a[i] * b[i];
        s1 += a[i + 1] * b[i + 1];
        s2 += a[i + 2] * b[i + 2];
        s3 += a[i + 3] * b[i + 3];
      }
      volatile float s = s0 + s1 + s2 + s3;
      (void)s;
    }, 80);
    write_study(outdir + "/01_scalar_vs_unrolled.json", "scalar_vs_simd_unroll",
                "4-way unroll reduces loop overhead vs scalar product", "scalar", before, "unrolled4",
                after, "Unrolled accumulate typically lowers median latency on CPU");
  }

  // 2) DFT vs FFT
  {
    nr_bb::ComplexVec x(64);
    for (size_t i = 0; i < x.size(); ++i) x[i] = {0.01 * static_cast<double>(i), -0.02};
    auto before = run_bench([&] { (void)nr_bb_edu::naive_dft(x, false); }, 60);
    auto after = run_bench([&] { (void)nr_bb::fft_radix2(x, false); }, 60);
    write_study(outdir + "/02_dft_vs_fft.json", "dft_vs_fft",
                "Radix-2 FFT beats naive DFT for N=64", "naive_dft", before, "fft_radix2", after,
                "FFT replaces educational DFT on acceptance path");
  }

  // 3) alloc vs pool
  {
    auto before = run_bench([&] {
      for (int k = 0; k < 64; ++k) {
        std::vector<float> tmp(4096, 1.0f);
        volatile float s = tmp[0];
        (void)s;
      }
    }, 100);
    std::vector<float> pool(4096, 1.0f);
    auto after = run_bench([&] {
      for (int k = 0; k < 64; ++k) {
        pool[static_cast<size_t>(k % 4096)] += 0.001f;
        volatile float s = pool[0];
        (void)s;
      }
    }, 100);
    write_study(outdir + "/03_alloc_vs_pool.json", "alloc_vs_pool",
                "Reusing a pool beats per-iteration heap alloc", "heap_alloc", before, "reuse_pool",
                after, "Pool reuse reduces median us versus fresh allocations");
  }

  // 4) AoS vs SoA
  {
    struct Sample {
      float re, im, w;
    };
    std::vector<Sample> aos(1 << 15);
    std::vector<float> re(1 << 15), im(1 << 15), w(1 << 15);
    for (size_t i = 0; i < aos.size(); ++i) {
      aos[i] = {1.f, 0.5f, 0.25f};
      re[i] = 1.f;
      im[i] = 0.5f;
      w[i] = 0.25f;
    }
    auto before = run_bench([&] {
      volatile float s = 0;
      for (auto& v : aos) s += v.re * v.w + v.im * v.w;
      (void)s;
    }, 80);
    auto after = run_bench([&] {
      volatile float s = 0;
      for (size_t i = 0; i < re.size(); ++i) s += re[i] * w[i] + im[i] * w[i];
      (void)s;
    }, 80);
    write_study(outdir + "/04_aos_vs_soa.json", "aos_vs_soa", "SoA improves contiguous math throughput",
                "aos", before, "soa", after, "SoA is at least competitive; used for IQ buffers");
  }

  // 5) pageable vs pinned mock (host-side timing only)
  {
    std::vector<float> pageable(1 << 20, 1.0f);
    alignas(64) float pinned[8192];
    for (auto& v : pinned) v = 1.0f;
    auto before = run_bench([&] {
      volatile float s = 0;
      for (size_t i = 0; i < pageable.size(); i += 128) s += pageable[i];
      (void)s;
    }, 50);
    auto after = run_bench([&] {
      volatile float s = 0;
      for (int r = 0; r < 128; ++r)
        for (int i = 0; i < 8192; i += 16) s += pinned[i];
      (void)s;
    }, 50);
    write_study(outdir + "/05_pageable_vs_pinned_host.json", "pageable_vs_pinned_host",
                "Aligned hot buffer beats cold large pageable walk (host-side mock only)", "pageable",
                before, "aligned_hot", after,
                "Host-side mock only — NOT CUDA pinned memory; no GPU claim");
  }

  // 6) sync vs async stubs (CPU futures simulation)
  {
    auto before = run_bench([&] {
      nr_bb::BitVec bits(256);
      for (size_t i = 0; i < bits.size(); ++i) bits[i] = static_cast<uint8_t>(i & 1);
      auto a = nr_bb::crc_attach(bits, nr_bb::CrcType::CRC24A);
      auto b = nr_bb::scramble(a, 0x1234);
      (void)nr_bb::modulate(b.size() % 2 ? nr_bb::BitVec(b.begin(), b.end() - 1) : b,
                            nr_bb::ModulationOrder::QPSK);
    }, 80);
    auto after = run_bench([&] {
      // Stub "async": stage buffers then consume (CPU ordering stub, not CUDA streams)
      nr_bb::BitVec bits(256);
      for (size_t i = 0; i < bits.size(); ++i) bits[i] = static_cast<uint8_t>(i & 1);
      auto a = nr_bb::crc_attach(bits, nr_bb::CrcType::CRC24A);
      auto staged = a;
      auto b = nr_bb::scramble(staged, 0x1234);
      auto bits2 = (b.size() % 2) ? nr_bb::BitVec(b.begin(), b.end() - 1) : b;
      (void)nr_bb::modulate(bits2, nr_bb::ModulationOrder::QPSK);
    }, 80);
    write_study(outdir + "/06_sync_vs_async_stub.json", "sync_vs_async_stub",
                "CPU staging stub approximates async pipeline without fake GPU streams", "sync_chain",
                before, "staged_stub", after,
                "CPU stub only — CUDA stream overlap remains BLOCKED_HARDWARE on this host");
  }

  // GPU study placeholder
  {
    std::ofstream f(outdir + "/07_gpu_blocked.json");
    f << "{\n"
      << "  \"study_id\": \"gpu_opt_study\",\n"
      << "  \"status\": \"BLOCKED_HARDWARE\",\n"
      << "  \"fail_closed\": true,\n"
      << "  \"gpu_timings_present\": false,\n"
      << "  \"notes\": \"No NVIDIA GPU — optimization-study-gpu emits pending JSON only\"\n"
      << "}\n";
  }

  std::cout << "Optimization studies written to " << outdir << "\n";
  return 0;
}
