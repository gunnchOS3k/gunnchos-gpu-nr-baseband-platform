# gunnchos GPU NR Baseband Platform

CPU-first, Aerial-aligned NR PUSCH-oriented baseband vertical slice with explicit
separation of **educational** substitutes vs **standards-path** PHY, plus CUDA
candidates and Gate 6 lab dry-runs.

**Not a claim of 3GPP conformance or carrier-grade status.**

## Host notes

- Apple M2 / CPU-only: build & test the CPU standards path.
- Missing NVIDIA GPU → `BLOCKED_HARDWARE` JSON (never invent GPU/NIC timings).
- Educational (16,8) LDPC / naive DFT / binary PAM live under `educational/` and
  are **not** sole Gate 4 acceptance criteria.

## Quick start

```bash
make bootstrap test
make gate4-cpu-reference optimization-study-cpu orchestrator gate6-dry-run
make gate4-cuda-build   # BLOCKED_HARDWARE on Mac
```

## Make targets

`bootstrap` `test` `reference-vectors` `sanitizers` `fuzz` `gate4-cpu-reference`
`gate4-cuda-build` `gate4-gpu` `optimization-study-cpu` `optimization-study-gpu`
`gate6-dry-run` `paper` `artifact` `reproduce-clean`

## Status vocabulary

`PASS` · `FAIL` · `BLOCKED_HARDWARE` · `PENDING_LAB` · `DOCUMENTED_IMPLEMENTATION` ·
`EDUCATIONAL_ONLY` · `DOI_PENDING` · `RELEASE_CANDIDATE_READY`

## License

MIT — see `LICENSE`. Cite via `CITATION.cff`.
