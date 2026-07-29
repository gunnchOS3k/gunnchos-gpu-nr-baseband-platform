#include "nr_bb_edu/binary_pam.hpp"
#include <cmath>
#include <limits>

namespace nr_bb_edu {

namespace {
double scale_for(nr_bb::ModulationOrder order) {
  switch (order) {
    case nr_bb::ModulationOrder::QPSK: return 1.0 / std::sqrt(2.0);
    case nr_bb::ModulationOrder::QAM16: return 1.0 / std::sqrt(10.0);
    case nr_bb::ModulationOrder::QAM64: return 1.0 / std::sqrt(42.0);
    case nr_bb::ModulationOrder::QAM256: return 1.0 / std::sqrt(170.0);
  }
  throw nr_bb::Error("unknown modulation");
}
int amp_from_bits(const nr_bb::BitVec& b, int start, int nbits) {
  int v = 0;
  for (int i = 0; i < nbits; ++i) v = (v << 1) | b[static_cast<size_t>(start + i)];
  const int levels = 1 << nbits;
  return 2 * v - (levels - 1);
}
}  // namespace

nr_bb::ComplexVec modulate_binary_pam(const nr_bb::BitVec& bits, nr_bb::ModulationOrder order) {
  const int m = static_cast<int>(order);
  nr_bb::require(bits.size() % static_cast<size_t>(m) == 0, "bit length not multiple of Qm");
  const int half = m / 2;
  const double s = scale_for(order);
  nr_bb::ComplexVec out;
  out.reserve(bits.size() / static_cast<size_t>(m));
  for (std::size_t i = 0; i < bits.size(); i += static_cast<size_t>(m)) {
    out.emplace_back(s * amp_from_bits(bits, static_cast<int>(i), half),
                     s * amp_from_bits(bits, static_cast<int>(i) + half, half));
  }
  return out;
}

nr_bb::SoftVec hard_constant_llr(const nr_bb::ComplexVec& syms, nr_bb::ModulationOrder order,
                                 double magnitude) {
  // Intentionally wrong: hard decisions with constant |LLR|.
  const int m = static_cast<int>(order);
  const int half = m / 2;
  const double s = scale_for(order);
  const int levels = 1 << half;
  nr_bb::BitVec hard;
  for (const auto& z : syms) {
    auto nearest = [&](double x) {
      int best = -(levels - 1);
      double best_d = std::numeric_limits<double>::infinity();
      for (int a = -(levels - 1); a <= (levels - 1); a += 2) {
        const double d = std::abs(x - s * a);
        if (d < best_d) {
          best_d = d;
          best = a;
        }
      }
      return best;
    };
    auto bits_from = [&](int amp) {
      int v = (amp + (levels - 1)) / 2;
      if (v < 0) v = 0;
      if (v >= levels) v = levels - 1;
      for (int i = half - 1; i >= 0; --i) hard.push_back(static_cast<uint8_t>((v >> i) & 1));
    };
    bits_from(nearest(z.real()));
    bits_from(nearest(z.imag()));
  }
  nr_bb::SoftVec llr(hard.size());
  for (std::size_t i = 0; i < hard.size(); ++i) llr[i] = (hard[i] == 0) ? magnitude : -magnitude;
  return llr;
}

}  // namespace nr_bb_edu
