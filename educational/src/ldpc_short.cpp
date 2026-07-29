#include "nr_bb_edu/ldpc_short.hpp"
#include <vector>

namespace nr_bb_edu {

namespace {
constexpr int N = 16;
constexpr int K = 8;
constexpr int M = 8;
const int P_COL[M][3] = {
    {0, 1, 2}, {1, 2, 3}, {2, 3, 4}, {3, 4, 5},
    {4, 5, 6}, {5, 6, 7}, {6, 7, 0}, {7, 0, 1},
};
void require_params(const ShortLdpcParams& p) {
  nr_bb::require(p.n == N && p.k == K, "educational LDPC only supports (16,8)");
}
int var_of(int r, int e) { return (e < 3) ? P_COL[r][e] : (K + r); }

nr_bb::BitVec encode_impl(const nr_bb::BitVec& info) {
  nr_bb::require(info.size() == K, "info length must be k=8");
  nr_bb::BitVec cw(N, 0);
  for (int i = 0; i < K; ++i) cw[static_cast<size_t>(i)] = info[static_cast<size_t>(i)];
  for (int r = 0; r < M; ++r) {
    uint8_t s = 0;
    for (int t = 0; t < 3; ++t) s ^= cw[static_cast<size_t>(P_COL[r][t])];
    cw[static_cast<size_t>(K + r)] = s;
  }
  return cw;
}

bool syndrome_ok(const nr_bb::BitVec& hard) {
  for (int r = 0; r < M; ++r) {
    uint8_t s = 0;
    for (int e = 0; e < 4; ++e) s ^= hard[static_cast<size_t>(var_of(r, e))];
    if (s) return false;
  }
  return true;
}
}  // namespace

nr_bb::BitVec ldpc_encode(const nr_bb::BitVec& info, const ShortLdpcParams& p) {
  require_params(p);
  return encode_impl(info);
}

nr_bb::BitVec ldpc_decode(const nr_bb::SoftVec& llr, const ShortLdpcParams& p) {
  require_params(p);
  nr_bb::require(llr.size() == static_cast<size_t>(N), "LLR length must be n=16");
  nr_bb::BitVec hard(N);
  for (int i = 0; i < N; ++i) hard[static_cast<size_t>(i)] = (llr[static_cast<size_t>(i)] >= 0.0) ? 0 : 1;
  for (int it = 0; it < p.max_iter; ++it) {
    if (syndrome_ok(hard)) break;
    std::vector<int> votes(N, 0);
    for (int r = 0; r < M; ++r) {
      uint8_t s = 0;
      for (int e = 0; e < 4; ++e) s ^= hard[static_cast<size_t>(var_of(r, e))];
      if (s) {
        for (int e = 0; e < 4; ++e) votes[static_cast<size_t>(var_of(r, e))] += 1;
      }
    }
    int best = 0;
    for (int i = 1; i < N; ++i)
      if (votes[static_cast<size_t>(i)] > votes[static_cast<size_t>(best)]) best = i;
    if (votes[static_cast<size_t>(best)] == 0) break;
    hard[static_cast<size_t>(best)] ^= 1;
  }
  return nr_bb::BitVec(hard.begin(), hard.begin() + K);
}

ShortLdpcReference make_reference(const nr_bb::BitVec& info, const ShortLdpcParams& p) {
  ShortLdpcReference r;
  r.info = info;
  r.codeword = ldpc_encode(info, p);
  return r;
}

}  // namespace nr_bb_edu
