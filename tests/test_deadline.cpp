#include <catch2/catch_test_macros.hpp>
#include "nr_bb/deadline.hpp"

TEST_CASE("numerology slot durations", "[deadline]") {
  REQUIRE(nr_bb::slot_duration_us_for_mu(0) == 1000.0);
  REQUIRE(nr_bb::slot_duration_us_for_mu(1) == 500.0);
  REQUIRE(nr_bb::slot_duration_us_for_mu(2) == 250.0);
}

TEST_CASE("deadline miss counting", "[deadline]") {
  std::vector<double> us = {10, 20, 30, 2000};
  auto r = nr_bb::evaluate_deadlines(us, nr_bb::NumerologyConfig{.mu = 0}, 1.0);
  REQUIRE(r.deadline_us == 1000.0);
  REQUIRE(r.deadline_misses == 1);
  REQUIRE(r.timing_source == "cpu_synthetic");
}
