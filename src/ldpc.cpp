#include "nr_bb/ldpc.hpp"
#include "nr_bb/ldpc_bg_tables.hpp"
#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

namespace nr_bb {

namespace {

constexpr int kLiftingSets[8][8] = {
    {2, 4, 8, 16, 32, 64, 128, 256},
    {3, 6, 12, 24, 48, 96, 192, 384},
    {5, 10, 20, 40, 80, 160, 320, -1},
    {7, 14, 28, 56, 112, 224, -1, -1},
    {9, 18, 36, 72, 144, 288, -1, -1},
    {11, 22, 44, 88, 176, 352, -1, -1},
    {13, 26, 52, 104, 208, -1, -1, -1},
    {15, 30, 60, 120, 240, -1, -1, -1},
};

struct Graph {
  int mb = 0, nb = 0, kb = 0, zc = 0, i_ls = 0;
  int filler = 0;
  std::vector<ldpc_tables::BgEdge> edges;
  std::string provenance;
};

int mod_shift(int v, int zc) {
  int m = v % zc;
  if (m < 0) m += zc;
  return m;
}

Graph make_graph(const LdpcParams& p) {
  Graph g;
  if (p.bg == BaseGraph::BG1) {
    g.mb = ldpc_tables::BG1_MB;
    g.nb = ldpc_tables::BG1_NB;
    g.kb = (p.kb > 0) ? p.kb : ldpc_tables::BG1_KB;
    g.edges.assign(ldpc_tables::BG1_EDGES, ldpc_tables::BG1_EDGES + ldpc_tables::BG1_EDGES_COUNT);
    g.provenance = "NR BG1 TS 38.212 via Sionna Apache-2.0 CSV tables";
  } else {
    g.mb = ldpc_tables::BG2_MB;
    g.nb = ldpc_tables::BG2_NB;
    g.kb = (p.kb > 0) ? p.kb : ldpc_tables::BG2_KB;
    g.edges.assign(ldpc_tables::BG2_EDGES, ldpc_tables::BG2_EDGES + ldpc_tables::BG2_EDGES_COUNT);
    g.provenance = "NR BG2 TS 38.212 via Sionna Apache-2.0 CSV tables";
  }

  if (p.zc > 0) {
    g.zc = p.zc;
    auto ils = i_ls_for_zc(p.zc);
    require(ils.has_value(), "Zc not in TS 38.212 Table 5.3.2-1");
    g.i_ls = (p.i_ls >= 0) ? p.i_ls : *ils;
  } else {
    require(p.info_bits > 0, "zc==0 requires info_bits for lifting selection");
    auto lift = select_lifting(p.info_bits, p.bg);
    g.zc = lift.zc;
    g.i_ls = (p.i_ls >= 0) ? p.i_ls : lift.i_ls;
  }
  require(g.i_ls >= 0 && g.i_ls <= 7, "i_ls out of range");
  require(g.kb > 0 && g.kb < g.nb, "invalid kb");

  if (p.mb_use > 0) {
    require(p.mb_use <= g.mb, "mb_use exceeds base graph");
    g.mb = p.mb_use;
    // Truncate to square parity: nb' = kb + mb' (rate-matched sub-graph).
    g.nb = g.kb + g.mb;
    std::vector<ldpc_tables::BgEdge> kept;
    for (const auto& e : g.edges) {
      if (e.row < g.mb && e.col < g.nb) kept.push_back(e);
    }
    g.edges.swap(kept);
  }

  const int k_ldpc = g.kb * g.zc;
  if (p.info_bits > 0) {
    require(p.info_bits <= k_ldpc, "info_bits exceeds kb*Zc");
    g.filler = k_ldpc - p.info_bits;
  }
  return g;
}

int N_of(const Graph& g) { return g.nb * g.zc; }
int K_of(const Graph& g) { return g.kb * g.zc; }
int M_of(const Graph& g) { return g.mb * g.zc; }

int lifted_var(int col_block, int shift, int row_within, int zc) {
  return col_block * zc + ((row_within + shift) % zc);
}

std::vector<uint8_t> build_H(const Graph& g) {
  const int M = M_of(g), N = N_of(g);
  std::vector<uint8_t> H(static_cast<size_t>(M * N), 0);
  for (const auto& e : g.edges) {
    const int raw = e.shift[g.i_ls];
    if (raw < 0) continue;
    const int sh = mod_shift(raw, g.zc);
    for (int z = 0; z < g.zc; ++z) {
      const int r = e.row * g.zc + z;
      const int c = lifted_var(e.col, sh, z, g.zc);
      H[static_cast<size_t>(r * N + c)] ^= 1;
    }
  }
  return H;
}

BitVec encode_ge(const BitVec& info_full, const Graph& g) {
  const int K = K_of(g), N = N_of(g), M = M_of(g);
  require(static_cast<int>(info_full.size()) == K, "info length must equal kb*zc (incl. filler)");
  auto H = build_H(g);

  std::vector<uint8_t> rhs(static_cast<size_t>(M), 0);
  for (int r = 0; r < M; ++r) {
    uint8_t s = 0;
    for (int k = 0; k < K; ++k) {
      if (H[static_cast<size_t>(r * N + k)] && info_full[static_cast<size_t>(k)]) s ^= 1;
    }
    rhs[static_cast<size_t>(r)] = s;
  }

  std::vector<uint8_t> A(static_cast<size_t>(M * M), 0);
  for (int r = 0; r < M; ++r)
    for (int c = 0; c < M; ++c) A[static_cast<size_t>(r * M + c)] = H[static_cast<size_t>(r * N + (K + c))];

  for (int col = 0; col < M; ++col) {
    int piv = col;
    while (piv < M && A[static_cast<size_t>(piv * M + col)] == 0) ++piv;
    require(piv < M, "parity submatrix singular — check Zc/i_ls/BG");
    if (piv != col) {
      for (int c = 0; c < M; ++c)
        std::swap(A[static_cast<size_t>(col * M + c)], A[static_cast<size_t>(piv * M + c)]);
      std::swap(rhs[static_cast<size_t>(col)], rhs[static_cast<size_t>(piv)]);
    }
    for (int r = 0; r < M; ++r) {
      if (r == col || A[static_cast<size_t>(r * M + col)] == 0) continue;
      for (int c = col; c < M; ++c) A[static_cast<size_t>(r * M + c)] ^= A[static_cast<size_t>(col * M + c)];
      rhs[static_cast<size_t>(r)] ^= rhs[static_cast<size_t>(col)];
    }
  }

  BitVec cw(static_cast<size_t>(N));
  for (int i = 0; i < K; ++i) cw[static_cast<size_t>(i)] = info_full[static_cast<size_t>(i)];
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
    const int raw = e.shift[g.i_ls];
    if (raw < 0) continue;
    const int sh = mod_shift(raw, g.zc);
    for (int z = 0; z < g.zc; ++z) {
      out.push_back({e.row * g.zc + z, lifted_var(e.col, sh, z, g.zc)});
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
  SoftVec app(static_cast<size_t>(N));

  auto hard_of = [&](const SoftVec& a) {
    BitVec h(static_cast<size_t>(N));
    for (int i = 0; i < N; ++i) h[static_cast<size_t>(i)] = (a[static_cast<size_t>(i)] >= 0) ? 0 : 1;
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

  for (int it = 0; it < max_iter; ++it) {
    for (int v = 0; v < N; ++v) {
      double sum = channel[static_cast<size_t>(v)];
      for (int ei : var_edges[static_cast<size_t>(v)]) sum += msg_c2v[static_cast<size_t>(ei)];
      app[static_cast<size_t>(v)] = sum;
      for (int ei : var_edges[static_cast<size_t>(v)]) msg_v2c[static_cast<size_t>(ei)] = sum - msg_c2v[static_cast<size_t>(ei)];
    }
    auto hard = hard_of(app);
    if (syndrome(hard)) return BitVec(hard.begin(), hard.begin() + K);
    for (int c = 0; c < M; ++c) {
      const auto& els = chk_edges[static_cast<size_t>(c)];
      for (int ei : els) {
        double min1 = std::numeric_limits<double>::infinity();
        int sign = 1;
        for (int ej : els) {
          if (ej == ei) continue;
          const double a = std::abs(msg_v2c[static_cast<size_t>(ej)]);
          if (a < min1) min1 = a;
          if (msg_v2c[static_cast<size_t>(ej)] < 0) sign = -sign;
        }
        msg_c2v[static_cast<size_t>(ei)] = static_cast<double>(sign) * min1 * 0.75;
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

BaseGraph select_basegraph(int k_bits, double coderate) {
  require(k_bits >= 12, "K too small for NR LDPC");
  if (k_bits <= 292) return BaseGraph::BG2;
  if (k_bits <= 3824 && coderate <= 0.67) return BaseGraph::BG2;
  if (coderate <= 0.25) return BaseGraph::BG2;
  return BaseGraph::BG1;
}

bool is_valid_lifting_size(int zc) { return i_ls_for_zc(zc).has_value(); }

std::optional<int> i_ls_for_zc(int zc) {
  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      if (kLiftingSets[i][j] == zc) return i;
    }
  }
  return std::nullopt;
}

LiftingChoice select_lifting(int k_bits, BaseGraph bg) {
  int kb_sel = 22;
  if (bg == BaseGraph::BG2) {
    if (k_bits > 640)
      kb_sel = 10;
    else if (k_bits > 560)
      kb_sel = 9;
    else if (k_bits > 192)
      kb_sel = 8;
    else
      kb_sel = 6;
  }
  int best_z = 0, best_ils = 0, best_val = 1000000;
  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      const int z = kLiftingSets[i][j];
      if (z < 0) continue;
      const int x = kb_sel * z;
      if (x >= k_bits && x < best_val) {
        best_val = x;
        best_z = z;
        best_ils = i;
      }
    }
  }
  require(best_z > 0, "no valid lifting size for K");
  LiftingChoice out;
  out.zc = best_z;
  out.i_ls = best_ils;
  out.kb = (bg == BaseGraph::BG1) ? ldpc_tables::BG1_KB : ldpc_tables::BG2_KB;
  return out;
}

LdpcGraphInfo ldpc_graph_info(const LdpcParams& p) {
  const auto g = make_graph(p);
  return LdpcGraphInfo{g.mb, g.nb, g.kb, g.zc, g.i_ls, N_of(g), K_of(g), g.filler, g.provenance};
}

BitVec ldpc_encode(const BitVec& info, const LdpcParams& p) {
  for (auto b : info) require(b == 0 || b == 1, "info bits must be 0/1");
  LdpcParams pp = p;
  if (pp.zc == 0 && pp.info_bits < 0) pp.info_bits = static_cast<int>(info.size());
  if (pp.info_bits < 0) pp.info_bits = static_cast<int>(info.size());
  const auto g = make_graph(pp);
  const int K = K_of(g);
  BitVec padded(static_cast<size_t>(K), 0);
  require(static_cast<int>(info.size()) <= K, "info longer than kb*Zc");
  // Place information bits then filler zeros (shortening).
  std::copy(info.begin(), info.end(), padded.begin());
  return encode_ge(padded, g);
}

BitVec ldpc_decode(const SoftVec& llr, const LdpcParams& p) {
  LdpcParams pp = p;
  if (pp.zc == 0) require(pp.info_bits > 0, "decode with zc==0 needs info_bits");
  return decode_minsum(llr, make_graph(pp), pp.max_iter);
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
