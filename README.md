# gunnchos GPU NR Baseband Platform

CPU-first, Aerial-aligned NR PUSCH-oriented baseband vertical slice with explicit
separation of **educational** substitutes vs **real NR** standards-path PHY, plus
CUDA candidates and Gate 6 lab dry-runs.

**Not a claim of 3GPP conformance or carrier-grade status.**

## Host notes

- Apple M2 / CPU-only: build & test the CPU standards path.
- Missing NVIDIA GPU → `BLOCKED_HARDWARE` / `BLOCKED_GPU_RUNNER` JSON (never invent timings).
- CompactQcLdpc + (16,8) LDPC live under `educational/` — **not** NR acceptance.
- Real BG1/BG2 tables: `third_party/sionna_ldpc_codes/` (Apache-2.0) → `ldpc_bg_tables.hpp`.

## Quick start

```bash
make bootstrap test
make validate-phy-independent
make cuda-correctness cuda-equivalence   # BLOCKED_HARDWARE on Mac
make gate4-cpu-reference optimization-study-cpu orchestrator gate6-dry-run
```

## Make targets

`bootstrap` `test` `reference-vectors` `sanitizers` `fuzz` `gate4-cpu-reference`
`gate4-cuda-build` `gate4-gpu` `optimization-study-cpu` `optimization-study-gpu`
`validate-phy-independent` `cuda-correctness` `cuda-equivalence`
`gate6-dry-run` `paper` `artifact` `reproduce-clean`

## Status vocabulary

`PASS` · `FAIL` · `BLOCKED_HARDWARE` · `BLOCKED_GPU_RUNNER` · `PENDING_LAB` ·
`DOCUMENTED_IMPLEMENTATION` · `EDUCATIONAL_ONLY` · `REFERENCE_DEPENDENCY_UNAVAILABLE` ·
`COMPARISON_PASS` / `COMPARISON_FAIL` · `DOI_PENDING` · `RELEASE_CANDIDATE_READY`

## License

MIT — see `LICENSE`. Third-party BG CSVs: Apache-2.0 (Sionna). Cite via `CITATION.cff`.
