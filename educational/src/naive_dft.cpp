#include "nr_bb_edu/naive_dft.hpp"
#include <cmath>
#include <numbers>

namespace nr_bb_edu {

nr_bb::ComplexVec naive_dft(const nr_bb::ComplexVec& in, bool inverse) {
  const std::size_t N = in.size();
  nr_bb::ComplexVec out(N);
  const double sign = inverse ? 1.0 : -1.0;
  const double scale = inverse ? (1.0 / static_cast<double>(N)) : 1.0;
  for (std::size_t k = 0; k < N; ++k) {
    nr_bb::Complex sum{0, 0};
    for (std::size_t n = 0; n < N; ++n) {
      const double ang = sign * 2.0 * std::numbers::pi * static_cast<double>(k) * static_cast<double>(n) /
                         static_cast<double>(N);
      sum += in[n] * nr_bb::Complex{std::cos(ang), std::sin(ang)};
    }
    out[k] = sum * scale;
  }
  return out;
}

}  // namespace nr_bb_edu
