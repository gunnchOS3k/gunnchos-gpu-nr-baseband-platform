#include "nr_bb/fft.hpp"
#include <cmath>
#include <numbers>

namespace nr_bb {

bool is_power_of_two(std::size_t n) { return n != 0 && (n & (n - 1)) == 0; }

ComplexVec fft_radix2(const ComplexVec& in, bool inverse) {
  const std::size_t N = in.size();
  require(is_power_of_two(N), "fft_radix2 requires power-of-two length");
  ComplexVec a = in;
  // Bit-reverse permutation
  for (std::size_t i = 1, j = 0; i < N; ++i) {
    std::size_t bit = N >> 1;
    for (; j & bit; bit >>= 1) j ^= bit;
    j ^= bit;
    if (i < j) std::swap(a[i], a[j]);
  }
  for (std::size_t len = 2; len <= N; len <<= 1) {
    const double ang = (inverse ? 2.0 : -2.0) * std::numbers::pi / static_cast<double>(len);
    const Complex wlen{std::cos(ang), std::sin(ang)};
    for (std::size_t i = 0; i < N; i += len) {
      Complex w{1.0, 0.0};
      for (std::size_t j = 0; j < len / 2; ++j) {
        Complex u = a[i + j];
        Complex v = a[i + j + len / 2] * w;
        a[i + j] = u + v;
        a[i + j + len / 2] = u - v;
        w *= wlen;
      }
    }
  }
  if (inverse) {
    const double inv = 1.0 / static_cast<double>(N);
    for (auto& z : a) z *= inv;
  }
  return a;
}

}  // namespace nr_bb
