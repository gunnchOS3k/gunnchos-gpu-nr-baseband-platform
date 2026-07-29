#include "nr_bb/modulation.hpp"
#include <cmath>
#include <limits>

namespace nr_bb {

int bits_per_symbol(ModulationOrder order) { return static_cast<int>(order); }

namespace {
double scale_for(ModulationOrder order) {
  switch (order) {
    case ModulationOrder::QPSK: return 1.0 / std::sqrt(2.0);
    case ModulationOrder::QAM16: return 1.0 / std::sqrt(10.0);
    case ModulationOrder::QAM64: return 1.0 / std::sqrt(42.0);
    case ModulationOrder::QAM256: return 1.0 / std::sqrt(170.0);
  }
  throw Error("unknown modulation");
}

int amp_from_bits(const BitVec& b, int start, int nbits) {
  // Gray-ish PAM: map bit groups to odd integers
  int v = 0;
  for (int i = 0; i < nbits; ++i) v = (v << 1) | b[start + i];
  // binary to Gray decode then to PAM levels is heavy; use direct binary PAM for education
  const int levels = 1 << nbits;
  return 2 * v - (levels - 1);
}

void bits_from_amp(int amp, int nbits, BitVec& out) {
  const int levels = 1 << nbits;
  int v = (amp + (levels - 1)) / 2;
  if (v < 0) v = 0;
  if (v >= levels) v = levels - 1;
  for (int i = nbits - 1; i >= 0; --i) out.push_back(static_cast<uint8_t>((v >> i) & 1));
}
}  // namespace

ComplexVec modulate(const BitVec& bits, ModulationOrder order) {
  const int m = bits_per_symbol(order);
  require(bits.size() % static_cast<size_t>(m) == 0, "bit length not multiple of Qm");
  const int half = m / 2;
  const double s = scale_for(order);
  ComplexVec out;
  out.reserve(bits.size() / m);
  for (std::size_t i = 0; i < bits.size(); i += static_cast<size_t>(m)) {
    const int re = amp_from_bits(bits, static_cast<int>(i), half);
    const int im = amp_from_bits(bits, static_cast<int>(i) + half, half);
    out.emplace_back(s * re, s * im);
  }
  return out;
}

BitVec demodulate_hard(const ComplexVec& syms, ModulationOrder order) {
  const int m = bits_per_symbol(order);
  const int half = m / 2;
  const double s = scale_for(order);
  const int levels = 1 << half;
  BitVec out;
  out.reserve(syms.size() * static_cast<size_t>(m));
  for (const auto& z : syms) {
    auto nearest = [&](double x) {
      int best = -(levels - 1);
      double best_d = std::numeric_limits<double>::infinity();
      for (int a = -(levels - 1); a <= (levels - 1); a += 2) {
        const double d = std::abs(x - s * a);
        if (d < best_d) { best_d = d; best = a; }
      }
      return best;
    };
    bits_from_amp(nearest(z.real()), half, out);
    bits_from_amp(nearest(z.imag()), half, out);
  }
  return out;
}

SoftVec demodulate_llr(const ComplexVec& syms, ModulationOrder order, double noise_var) {
  require(noise_var > 0, "noise_var must be > 0");
  // Soft approx: map hard distance to LLR magnitude
  const BitVec hard = demodulate_hard(syms, order);
  SoftVec llr(hard.size());
  const double mag = 2.0 / noise_var;
  for (std::size_t i = 0; i < hard.size(); ++i) llr[i] = (hard[i] == 0) ? mag : -mag;
  return llr;
}

}  // namespace nr_bb
