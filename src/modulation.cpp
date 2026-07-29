#include "nr_bb/modulation.hpp"
#include <cmath>
#include <limits>
#include <vector>

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

/** Binary → Gray */
int bin_to_gray(int v) { return v ^ (v >> 1); }
/** Gray → binary */
int gray_to_bin(int g) {
  int b = g;
  while (g >>= 1) b ^= g;
  return b;
}

/** PAM amplitude from Gray-coded nbits (MSB first) → odd integers.
 *  Polarity matches TS 38.211-style (1−2b): all-zero Gray → most positive. */
int gray_pam_amp(const BitVec& bits, int start, int nbits) {
  int g = 0;
  for (int i = 0; i < nbits; ++i) g = (g << 1) | bits[static_cast<size_t>(start + i)];
  const int v = gray_to_bin(g);
  const int levels = 1 << nbits;
  return (levels - 1) - 2 * v;
}

void gray_bits_from_amp(int amp, int nbits, BitVec& out) {
  const int levels = 1 << nbits;
  int v = ((levels - 1) - amp) / 2;
  if (v < 0) v = 0;
  if (v >= levels) v = levels - 1;
  const int g = bin_to_gray(v);
  for (int i = nbits - 1; i >= 0; --i) out.push_back(static_cast<uint8_t>((g >> i) & 1));
}

Complex symbol_from_index(int idx, ModulationOrder order) {
  const int m = bits_per_symbol(order);
  const int half = m / 2;
  const int levels = 1 << half;
  BitVec bits(static_cast<size_t>(m));
  for (int i = m - 1; i >= 0; --i) bits[static_cast<size_t>(m - 1 - i)] = static_cast<uint8_t>((idx >> i) & 1);
  int g_i = 0, g_q = 0;
  for (int i = 0; i < half; ++i) g_i = (g_i << 1) | bits[static_cast<size_t>(i)];
  for (int i = 0; i < half; ++i) g_q = (g_q << 1) | bits[static_cast<size_t>(half + i)];
  const int v_i = gray_to_bin(g_i);
  const int v_q = gray_to_bin(g_q);
  const double s = scale_for(order);
  return {s * ((levels - 1) - 2 * v_i), s * ((levels - 1) - 2 * v_q)};
}

}  // namespace

std::vector<Complex> constellation(ModulationOrder order) {
  const int M = 1 << bits_per_symbol(order);
  std::vector<Complex> c;
  c.reserve(static_cast<size_t>(M));
  for (int i = 0; i < M; ++i) c.push_back(symbol_from_index(i, order));
  return c;
}

double constellation_avg_energy(ModulationOrder order) {
  const auto c = constellation(order);
  double e = 0;
  for (const auto& z : c) e += std::norm(z);
  return e / static_cast<double>(c.size());
}

ComplexVec modulate(const BitVec& bits, ModulationOrder order) {
  const int m = bits_per_symbol(order);
  require(bits.size() % static_cast<size_t>(m) == 0, "bit length not multiple of Qm");
  const int half = m / 2;
  const double s = scale_for(order);
  ComplexVec out;
  out.reserve(bits.size() / static_cast<size_t>(m));
  for (std::size_t i = 0; i < bits.size(); i += static_cast<size_t>(m)) {
    const int re = gray_pam_amp(bits, static_cast<int>(i), half);
    const int im = gray_pam_amp(bits, static_cast<int>(i) + half, half);
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
        if (d < best_d) {
          best_d = d;
          best = a;
        }
      }
      return best;
    };
    gray_bits_from_amp(nearest(z.real()), half, out);
    gray_bits_from_amp(nearest(z.imag()), half, out);
  }
  return out;
}

SoftVec demodulate_llr(const ComplexVec& syms, ModulationOrder order, double noise_var) {
  require(noise_var > 0, "noise_var must be > 0");
  const int m = bits_per_symbol(order);
  const int M = 1 << m;
  const auto alphabet = constellation(order);
  SoftVec llr;
  llr.reserve(syms.size() * static_cast<size_t>(m));
  const double inv2s = 1.0 / (2.0 * noise_var);
  for (const auto& y : syms) {
    for (int k = 0; k < m; ++k) {
      double min0 = std::numeric_limits<double>::infinity();
      double min1 = std::numeric_limits<double>::infinity();
      for (int s = 0; s < M; ++s) {
        const double d2 = std::norm(y - alphabet[static_cast<size_t>(s)]);
        if (((s >> (m - 1 - k)) & 1) == 0) {
          if (d2 < min0) min0 = d2;
        } else {
          if (d2 < min1) min1 = d2;
        }
      }
      llr.push_back(inv2s * (min1 - min0));
    }
  }
  return llr;
}

}  // namespace nr_bb
