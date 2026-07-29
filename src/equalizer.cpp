#include "nr_bb/equalizer.hpp"
#include <cmath>
#include <vector>

namespace nr_bb {

namespace {
// Small dense complex linear algebra for up to 4x4.
using Mat = std::vector<Complex>;  // row-major r*c

Mat mat_mul(const Mat& A, int ar, int ac, const Mat& B, int br, int bc) {
  require(ac == br, "matmul shape");
  Mat C(static_cast<size_t>(ar * bc), Complex{0, 0});
  for (int i = 0; i < ar; ++i)
    for (int k = 0; k < ac; ++k)
      for (int j = 0; j < bc; ++j)
        C[static_cast<size_t>(i * bc + j)] += A[static_cast<size_t>(i * ac + k)] * B[static_cast<size_t>(k * bc + j)];
  return C;
}

Mat mat_herm(const Mat& A, int r, int c) {
  Mat AH(static_cast<size_t>(c * r));
  for (int i = 0; i < r; ++i)
    for (int j = 0; j < c; ++j) AH[static_cast<size_t>(j * r + i)] = std::conj(A[static_cast<size_t>(i * c + j)]);
  return AH;
}

Mat mat_eye(int n, Complex scale = {1, 0}) {
  Mat I(static_cast<size_t>(n * n), Complex{0, 0});
  for (int i = 0; i < n; ++i) I[static_cast<size_t>(i * n + i)] = scale;
  return I;
}

// Invert n x n via Gauss-Jordan (n<=4).
Mat mat_inv(Mat A, int n) {
  Mat aug(static_cast<size_t>(n * 2 * n), Complex{0, 0});
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) aug[static_cast<size_t>(i * 2 * n + j)] = A[static_cast<size_t>(i * n + j)];
    aug[static_cast<size_t>(i * 2 * n + n + i)] = Complex{1, 0};
  }
  for (int col = 0; col < n; ++col) {
    int piv = col;
    double best = std::abs(aug[static_cast<size_t>(piv * 2 * n + col)]);
    for (int r = col + 1; r < n; ++r) {
      const double v = std::abs(aug[static_cast<size_t>(r * 2 * n + col)]);
      if (v > best) { best = v; piv = r; }
    }
    require(best > 1e-14, "singular matrix in equalizer");
    if (piv != col) {
      for (int j = 0; j < 2 * n; ++j)
        std::swap(aug[static_cast<size_t>(col * 2 * n + j)], aug[static_cast<size_t>(piv * 2 * n + j)]);
    }
    const Complex diag = aug[static_cast<size_t>(col * 2 * n + col)];
    for (int j = 0; j < 2 * n; ++j) aug[static_cast<size_t>(col * 2 * n + j)] /= diag;
    for (int r = 0; r < n; ++r) {
      if (r == col) continue;
      const Complex f = aug[static_cast<size_t>(r * 2 * n + col)];
      for (int j = 0; j < 2 * n; ++j) aug[static_cast<size_t>(r * 2 * n + j)] -= f * aug[static_cast<size_t>(col * 2 * n + j)];
    }
  }
  Mat inv(static_cast<size_t>(n * n));
  for (int i = 0; i < n; ++i)
    for (int j = 0; j < n; ++j) inv[static_cast<size_t>(i * n + j)] = aug[static_cast<size_t>(i * 2 * n + n + j)];
  return inv;
}

ComplexVec mat_vec(const Mat& A, int r, int c, const ComplexVec& x) {
  require(static_cast<int>(x.size()) == c, "mat_vec");
  ComplexVec y(static_cast<size_t>(r), Complex{0, 0});
  for (int i = 0; i < r; ++i)
    for (int j = 0; j < c; ++j) y[static_cast<size_t>(i)] += A[static_cast<size_t>(i * c + j)] * x[static_cast<size_t>(j)];
  return y;
}
}  // namespace

ComplexVec equalize_sample(const ComplexVec& y, const std::vector<Complex>& H, int n_rx, int n_tx,
                           const EqualizerConfig& cfg) {
  require(static_cast<int>(y.size()) == n_rx, "y size");
  require(static_cast<int>(H.size()) == n_rx * n_tx, "H size");
  require(n_tx >= 1 && n_tx <= 4 && n_rx >= 1 && n_rx <= 4, "MIMO size 1..4");

  const Mat Hh = mat_herm(H, n_rx, n_tx);           // n_tx x n_rx
  Mat gram = mat_mul(Hh, n_tx, n_rx, H, n_rx, n_tx); // n_tx x n_tx
  double reg = 0.0;
  if (cfg.type == EqualizerType::MMSE) reg = cfg.noise_var;
  if (cfg.type == EqualizerType::RZF) reg = cfg.rzf_delta;
  if (reg != 0.0) {
    auto I = mat_eye(n_tx, Complex{reg, 0});
    for (std::size_t i = 0; i < gram.size(); ++i) gram[i] += I[i];
  }
  const Mat inv = mat_inv(gram, n_tx);
  const Mat W = mat_mul(inv, n_tx, n_tx, Hh, n_tx, n_rx);  // n_tx x n_rx
  return mat_vec(W, n_tx, n_rx, y);
}

std::vector<ComplexVec> equalize_stream(const std::vector<ComplexVec>& y_rx,
                                        const std::vector<Complex>& H, int n_tx,
                                        const EqualizerConfig& cfg) {
  require(!y_rx.empty(), "empty rx");
  const int n_rx = static_cast<int>(y_rx.size());
  const std::size_t T = y_rx[0].size();
  std::vector<ComplexVec> x_hat(static_cast<size_t>(n_tx), ComplexVec(T));
  for (std::size_t t = 0; t < T; ++t) {
    ComplexVec y(static_cast<size_t>(n_rx));
    for (int r = 0; r < n_rx; ++r) y[static_cast<size_t>(r)] = y_rx[static_cast<size_t>(r)][t];
    auto x = equalize_sample(y, H, n_rx, n_tx, cfg);
    for (int c = 0; c < n_tx; ++c) x_hat[static_cast<size_t>(c)][t] = x[static_cast<size_t>(c)];
  }
  return x_hat;
}

}  // namespace nr_bb
