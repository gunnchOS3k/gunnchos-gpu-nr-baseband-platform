#include "nr_bb/ldpc.hpp"
#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

namespace nr_bb {

namespace {

struct Edge {
  int row;    // block row
  int col;    // block col
  int shift;  // circular shift mod Zc
};

/*
 * Compact NR-style BG scaffolds (research-sized). Edge shifts are self-authored
 * under MIT for CI — QC lifting matches TS 38.212 mechanics.
 * Parity columns (kb .. nb-1) form a lifted identity (shift-0 diagonal) so
 * H[:,kb:) is always invertible for Zc in {2,4,8}.
 * BG1-compact: mb=4, nb=10, kb=6
 * BG2-compact: mb=4, nb=8,  kb=4
 */
const Edge BG1_EDGES[] = {
    {0, 0, 0}, {0, 1, 1}, {0, 2, 0}, {0, 6, 0},
    {1, 0, 1}, {1, 3, 0}, {1, 4, 1}, {1, 7, 0},
    {2, 1, 0}, {2, 2, 1}, {2, 5, 0}, {2, 8, 0},
    {3, 0, 0}, {3, 3, 1}, {3, 5, 1}, {3, 9, 0},
};
const Edge BG2_EDGES[] = {
    {0, 0, 0}, {0, 1, 1}, {0, 2, 0}, {0, 4, 0},
    {1, 0, 1}, {1, 1, 0}, {1, 3, 1}, {1, 5, 0},
    {2, 1, 0}, {2, 2, 1}, {2, 3, 0}, {2, 6, 0},
    {3, 0, 0}, {3, 2, 1}, {3, 3, 1}, {3, 7, 0},
};

struct Graph {
  int mb, nb, kb, zc;
  std::vector<Edge> edges;
  std::string provenance;
};

Graph make_graph(const LdpcParams& p) {
  Graph g;
  g.zc = p.zc;
  require(p.zc == 2 || p.zc == 4 || p.zc == 8, "scaffold Zc must be 2,4, or 8");
  if (p.bg == BaseGraph::BG1) {
    g.mb = 4;
    g.nb = 10;
    g.kb = (p.info_cols > 0) ? p.info_cols : 6;
    g.edges.assign(std::begin(BG1_EDGES), std::end(BG1_EDGES));
    g.provenance = "BG1-compact scaffold (TS 38.212 §5.3.2 QC lifting mechanics)";
  } else {
    g.mb = 4;
    g.nb = 8;
    g.kb = (p.info_cols > 0) ? p.info_cols : 4;
    g.edges.assign(std::begin(BG2_EDGES), std::end(BG2_EDGES));
    g.provenance = "BG2-compact scaffold (TS 38.212 §5.3.2 QC lifting mechanics)";
  }
  require(g.kb > 0 && g.kb < g.nb, "invalid kb");
  require(g.nb - g.kb == g.mb, "scaffold requires nb-kb == mb");
  return g;
}

int N_of(const Graph& g) { return g.nb * g.zc; }
int K_of(const Graph& g) { return g.kb * g.zc; }
int M_of(const Graph& g) { return g.mb * g.zc; }

/** Bit index of circularly shifted connection. */
int lifted_var(int col_block, int shift, int row_within, int zc) {
  return col_block * zc + ((row_within + shift) % zc);
}

/** Build dense H over GF(2): M rows, N cols. */
std::vector<uint8_t> build_H(const Graph& g) {
  const int M = M_of(g), N = N_of(g);
  std::vector<uint8_t> H(static_cast<size_t>(M * N), 0);
  for (const auto& e : g.edges) {
    for (int z = 0; z < g.zc; ++z) {
      const int r = e.row * g.zc + z;
      const int c = lifted_var(e.col, e.shift, z, g.zc);
      H[static_cast<size_t>(r * N + c)] ^= 1;
    }
  }
  return H;
}

BitVec encode_ge(const BitVec& info, const Graph& g) {
  const int K = K_of(g), N = N_of(g), M = M_of(g);
  require(static_cast<int>(info.size()) == K, "info length must equal kb*zc");
  auto H = build_H(g);
  // Systematic: c = [info | parity], solve H[:,K:] p = H[:,:K] info
  std::vector<uint8_t> rhs(static_cast<size_t>(M), 0);
  for (int r = 0; r < M; ++r) {
    uint8_t s = 0;
    for (int k = 0; k < K; ++k) {
      if (H[static_cast<size_t>(r * N + k)] && info[static_cast<size_t>(k)]) s ^= 1;
    }
    rhs[static_cast<size_t>(r)] = s;
  }
  // Extract parity submatrix A = H[:,K:] size MxM
  std::vector<uint8_t> A(static_cast<size_t>(M * M), 0);
  for (int r = 0; r < M; ++r)
    for (int c = 0; c < M; ++c) A[static_cast<size_t>(r * M + c)] = H[static_cast<size_t>(r * N + (K + c))];

  // Augment A|rhs and Gaussian-eliminate over GF(2)
  for (int col = 0; col < M; ++col) {
    int piv = col;
    while (piv < M && A[static_cast<size_t>(piv * M + col)] == 0) ++piv;
    require(piv < M, "parity submatrix singular — scaffold graph invalid for Zc");
    if (piv != col) {
      for (int c = 0; c < M; ++c) std::swap(A[static_cast<size_t>(col * M + c)], A[static_cast<size_t>(piv * M + c)]);
      std::swap(rhs[static_cast<size_t>(col)], rhs[static_cast<size_t>(piv)]);
    }
    for (int r = 0; r < M; ++r) {
      if (r == col || A[static_cast<size_t>(r * M + col)] == 0) continue;
      for (int c = col; c < M; ++c) A[static_cast<size_t>(r * M + c)] ^= A[static_cast<size_t>(col * M + c)];
      rhs[static_cast<size_t>(r)] ^= rhs[static_cast<size_t>(col)];
    }
  }
  BitVec cw(static_cast<size_t>(N));
  for (int i = 0; i < K; ++i) cw[static_cast<size_t>(i)] = info[static_cast<size_t>(i)];
  for (int i = 0; i < M; ++i) cw[static_cast<size_t>(K + i)] = rhs[static_cast<size_t>(i)];
  return cw;
}

struct SparseEdge {
  int check;
  int var;
};

std::vector<SparseEdge> expand_edges(const Graph& g) {
  std::vector<SparseEdge> out;
  for (const auto& e : g.edges) {
    for (int z = 0; z < g.zc; ++z) {
      out.push_back({e.row * g.zc + z, lifted_var(e.col, e.shift, z, g.zc)});
    }
  }
  return out;
}

BitVec decode_minsum(const SoftVec& llr, const Graph& g, int max_iter) {
  const int N = N_of(g), M = M_of(g), K = K_of(g);
  require(static_cast<int>(llr.size()) == N, "LLR length must equal nb*zc");
  auto edges = expand_edges(g);
  std::vector<std::vector<int>> var_edges(static_cast<size_t>(N));
  std::vector<std::vector<int>> chk_edges(static_cast<size_t>(M));
  for (int i = 0; i < static_cast<int>(edges.size()); ++i) {
    var_edges[static_cast<size_t>(edges[static_cast<size_t>(i)].var)].push_back(i);
    chk_edges[static_cast<size_t>(edges[static_cast<size_t>(i)].check)].push_back(i);
  }
  SoftVec channel = llr;
  SoftVec msg_c2v(edges.size(), 0.0);
  SoftVec msg_v2c(edges.size(), 0.0);

  auto hard_of = [&](const SoftVec& app) {
    BitVec h(static_cast<size_t>(N));
    for (int i = 0; i < N; ++i) h[static_cast<size_t>(i)] = (app[static_cast<size_t>(i)] >= 0) ? 0 : 1;
    return h;
  };

  auto syndrome = [&](const BitVec& hard) {
    for (int c = 0; c < M; ++c) {
      uint8_t s = 0;
      for (int ei : chk_edges[static_cast<size_t>(c)]) s ^= hard[static_cast<size_t>(edges[static_cast<size_t>(ei)].var)];
      if (s) return false;
    }
    return true;
  };

  SoftVec app(static_cast<size_t>(N));
  for (int it = 0; it < max_iter; ++it) {
    // VN update
    for (int v = 0; v < N; ++v) {
      double sum = channel[static_cast<size_t>(v)];
      for (int ei : var_edges[static_cast<size_t>(v)]) sum += msg_c2v[static_cast<size_t>(ei)];
      app[static_cast<size_t>(v)] = sum;
      for (int ei : var_edges[static_cast<size_t>(v)]) {
        msg_v2c[static_cast<size_t>(ei)] = sum - msg_c2v[static_cast<size_t>(ei)];
      }
    }
    auto hard = hard_of(app);
    if (syndrome(hard)) {
      return BitVec(hard.begin(), hard.begin() + K);
    }
    // CN min-sum
    for (int c = 0; c < M; ++c) {
      const auto& els = chk_edges[static_cast<size_t>(c)];
      for (int ei : els) {
        double min1 = std::numeric_limits<double>::infinity();
        double min2 = std::numeric_limits<double>::infinity();
        int sign = 1;
        for (int ej : els) {
          if (ej == ei) continue;
          const double a = std::abs(msg_v2c[static_cast<size_t>(ej)]);
          if (a < min1) {
            min2 = min1;
            min1 = a;
          } else if (a < min2) {
            min2 = a;
          }
          if (msg_v2c[static_cast<size_t>(ej)] < 0) sign = -sign;
        }
        msg_c2v[static_cast<size_t>(ei)] = static_cast<double>(sign) * min1 * 0.75;  // normalized min-sum
        (void)min2;
      }
    }
  }
  for (int v = 0; v < N; ++v) {
    double sum = channel[static_cast<size_t>(v)];
    for (int ei : var_edges[static_cast<size_t>(v)]) sum += msg_c2v[static_cast<size_t>(ei)];
    app[static_cast<size_t>(v)] = sum;
  }
  auto hard = hard_of(app);
  return BitVec(hard.begin(), hard.begin() + K);
}

}  // namespace

LdpcGraphInfo ldpc_graph_info(const LdpcParams& p) {
  const auto g = make_graph(p);
  return LdpcGraphInfo{g.mb, g.nb, g.kb, g.zc, g.provenance};
}

BitVec ldpc_encode(const BitVec& info, const LdpcParams& p) {
  for (auto b : info) require(b == 0 || b == 1, "info bits must be 0/1");
  return encode_ge(info, make_graph(p));
}

BitVec ldpc_decode(const SoftVec& llr, const LdpcParams& p) {
  return decode_minsum(llr, make_graph(p), p.max_iter);
}

bool ldpc_syndrome_ok(const BitVec& codeword, const LdpcParams& p) {
  const auto g = make_graph(p);
  require(static_cast<int>(codeword.size()) == N_of(g), "codeword length");
  auto H = build_H(g);
  const int M = M_of(g), N = N_of(g);
  for (int r = 0; r < M; ++r) {
    uint8_t s = 0;
    for (int c = 0; c < N; ++c)
      if (H[static_cast<size_t>(r * N + c)]) s ^= codeword[static_cast<size_t>(c)];
    if (s) return false;
  }
  return true;
}

LdpcReferenceVector ldpc_make_reference(const BitVec& info, const LdpcParams& p) {
  LdpcReferenceVector r;
  r.info = info;
  r.params = p;
  r.codeword = ldpc_encode(info, p);
  r.provenance = ldpc_graph_info(p).provenance + "; self-generated";
  return r;
}

}  // namespace nr_bb
