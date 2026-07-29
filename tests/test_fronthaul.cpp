#include <catch2/catch_test_macros.hpp>
#include "nr_bb/fronthaul.hpp"
#include <filesystem>

TEST_CASE("fronthaul loopback and pcap", "[fh]") {
  nr_bb::FhImpairments imp;
  imp.loss_prob = 0.0;
  imp.corrupt_prob = 0.0;
  nr_bb::FronthaulEmulator em(imp);
  nr_bb::ComplexVec iq = {{1, 0}, {0, 1}};
  auto p = em.make_u_plane(iq, 7, 1, 123456789ULL);
  REQUIRE(p.meta.plane == nr_bb::Plane::U);
  auto lb = em.loopback({p, p});
  REQUIRE(lb.size() == 2);
  const auto path = "results/benchmarks/fh_loopback_test.pcap";
  std::filesystem::create_directories("results/benchmarks");
  em.write_pcap_dump(path, lb);
  REQUIRE(std::filesystem::file_size(path) > 24);
}

TEST_CASE("fronthaul loss impairment", "[fh]") {
  nr_bb::FhImpairments imp;
  imp.loss_prob = 1.0;
  imp.seed = 42;
  nr_bb::FronthaulEmulator em(imp);
  auto p = em.make_u_plane({{1, 0}}, 1, 1, 1);
  auto out = em.apply_impairments({p, p, p});
  REQUIRE(out.empty());
}
