#include "nr_bb/ldpc.hpp"
#include <vector>
#include <cmath>

namespace nr_bb {

namespace {
constexpr int N = 16;
constexpr int K = 8;
constexpr int M = 8;

const int P_COL[M][3] = {
    {0, 1, 2}, {1, 2, 3}, {2, 3, 4}, {3, 4, 5},
    {4, 5, 6}, {5, 6, 7}, {6, 7, 0}, {7, 0, 1},
};

void require_params(const LdpcParams& p) {
  require(p.n == N && p.k == K, "educational LDPC only supports (16,8)");
}

int var_of(int r, int e) { return (e < 3) ? P_COL[r][e] : (K + r); }

BitVec encode_impl(const BitVec& info) {
  require(info.size() == K, "info length must be k=8");
  BitVec cw(N, 0);
  for (int i = 0; i < K; ++i) {
    require(info[i] == 0 || info[i] == 1, "info bits must be 0/1");
    cw[static_cast<size_t>(i)] = info[static_cast<size_t>(i)];
  }
  for (int r = 0; r < M; ++r) {
    uint8_t s = 0;
    for (int t = 0; t < 3; ++t) s ^= cw[static_cast<size_t>(P_COL[r][t])];
    cw[static_cast<size_t>(K + r)] = s;
  }
  return cw;
}

bool syndrome_ok(const BitVec& hard) {
  for (int r = 0; r < M; ++r) {
    uint8_t s = 0;
    for (int e = 0; e < 4; ++e) s ^= hard[static_cast<size_t>(var_of(r, e))];
    if (s) return false;
  }
  return true;
}
}  // namespace

BitVec ldpc_encode(const BitVec& info, const LdpcParams& p) {
  require_params(p);
  return encode_impl(info);
}

BitVec ldpc_decode(const SoftVec& llr, const LdpcParams& p) {
  require_params(p);
  require(llr.size() == static_cast<size_t>(N), "LLR length must be n=16");

  BitVec hard(N);
  for (int i = 0; i < N; ++i) hard[static_cast<size_t>(i)] = (llr[static_cast<size_t>(i)] >= 0.0) ? 0 : 1;

  // Gallager bit-flipping for educational short code
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
    hard[static_cast<size_t>(best)] = static_cast<uint8_t>(1 - hard[static_cast<size_t>(best)]);
  }

  BitVec info(K);
  for (int i = 0; i < K; ++i) info[static_cast<size_t>(i)] = hard[static_cast<size_t>(i)];
  return info;
}

LdpcReferenceVector ldpc_make_reference(const BitVec& info, const LdpcParams& p) {
  LdpcReferenceVector v;
  v.info = info;
  v.codeword = ldpc_encode(info, p);
  v.provenance = "self-generated educational (16,8) LDPC";
  return v;
}

}  // namespace nr_bb
