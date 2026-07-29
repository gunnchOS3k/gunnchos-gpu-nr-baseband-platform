# LDPC source and license audit

**Status:** audited for Track C (real NR depth)  
**Host note:** Apple M2 — no GPU claim attached to LDPC correctness.

## Inventory

| Artifact | Location | License / provenance | Acceptance role |
|----------|----------|----------------------|-----------------|
| Compact NR-**style** QC scaffolds (mb=4) | `educational/nr_style_compact_ldpc/` (`CompactQcLdpc`) | MIT, self-authored edges | **EDUCATIONAL_ONLY** — must **not** be final NR acceptance |
| Short (16,8) LDPC | `educational/` (`ldpc_short`) | MIT | EDUCATIONAL_ONLY |
| Real NR BG1/BG2 shift tables | `third_party/sionna_ldpc_codes/*.csv` → `include/nr_bb/ldpc_bg_tables.hpp` | Apache-2.0 (Sionna) encoding of TS 38.212 Tables 5.3.2-2/3 | **Standards-path** acceptance LDPC |
| Encode / min-sum / RM / HARQ glue | `include/nr_bb/ldpc.hpp`, `src/ldpc.cpp`, RM/HARQ | MIT (this repo); algorithms public-domain / 3GPP-described | Standards-path |
| NVIDIA Aerial / cuBB | — | **Not present** | N/A |

## Compact QC relocation (mandatory)

The previous `src/ldpc.cpp` “BG1/BG2 compact scaffold” was **research-sized**
(mb=4, nb∈{8,10}) and is **not** a bit-exact TS 38.212 base graph.

It now lives under:

```text
educational/nr_style_compact_ldpc/
  include/nr_bb_edu/compact_qc_ldpc.hpp
  src/compact_qc_ldpc.cpp
  README.md
```

API prefix: `nr_bb_edu::CompactQcLdpc*` — labeled `EDUCATIONAL_ONLY`.

## Real NR path

- Dimensions: BG1 46×68 (Kb=22), BG2 42×52 (Kb=10).
- Lifting sizes: TS 38.212 Table 5.3.2-1 (all Zc families).
- BG selection: §5.2.2 rules (K / rate thresholds).
- Filler bits + CRC24B on multi-CB segmentation; rate matching RVs per §5.4.2.1-2 coeffs.
- Iterative normalized min-sum decode; HARQ soft combine via `HarqBuffer`.

## Forbidden claims

- Do **not** cite CompactQcLdpc as NR conformance.
- Do **not** invent GPU LDPC timings on this host (`BLOCKED_HARDWARE`).
- Do **not** vendor proprietary Aerial kernels.
