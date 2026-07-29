#include <catch2/catch_test_macros.hpp>
#include "nr_bb/dmrs.hpp"

TEST_CASE("DMRS grid placement", "[dmrs]") {
  nr_bb::DmrsConfig cfg;
  auto g = nr_bb::generate_dmrs(cfg);
  REQUIRE(g.sequence.size() == static_cast<size_t>(cfg.n_sc));
  for (int k = 0; k < cfg.n_sc; ++k) {
    REQUIRE(g.grid[static_cast<size_t>(cfg.dmrs_symb)][static_cast<size_t>(k)] ==
            g.sequence[static_cast<size_t>(k)]);
  }
}
