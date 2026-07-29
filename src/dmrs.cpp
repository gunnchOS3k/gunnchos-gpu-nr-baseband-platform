#include "nr_bb/dmrs.hpp"
#include "nr_bb/scrambling.hpp"
#include <cmath>

namespace nr_bb {

DmrsGrid generate_dmrs(const DmrsConfig& cfg) {
  require(cfg.n_sc > 0 && cfg.n_symb > 0, "invalid DMRS dimensions");
  require(cfg.dmrs_symb >= 0 && cfg.dmrs_symb < cfg.n_symb, "dmrs_symb out of range");
  DmrsGrid g;
  g.grid.assign(static_cast<size_t>(cfg.n_symb), ComplexVec(static_cast<size_t>(cfg.n_sc), Complex{0, 0}));
  const auto bits = gold_sequence(static_cast<size_t>(cfg.n_sc) * 2, cfg.c_init);
  const double s = 1.0 / std::sqrt(2.0);
  g.sequence.resize(static_cast<size_t>(cfg.n_sc));
  for (int k = 0; k < cfg.n_sc; ++k) {
    const double re = (bits[static_cast<size_t>(2 * k)] == 0) ? 1.0 : -1.0;
    const double im = (bits[static_cast<size_t>(2 * k + 1)] == 0) ? 1.0 : -1.0;
    g.sequence[static_cast<size_t>(k)] = Complex{s * re, s * im};
    g.grid[static_cast<size_t>(cfg.dmrs_symb)][static_cast<size_t>(k)] = g.sequence[static_cast<size_t>(k)];
  }
  return g;
}

}  // namespace nr_bb
