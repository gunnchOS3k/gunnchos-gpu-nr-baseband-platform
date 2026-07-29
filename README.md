# gunnchos GPU NR Baseband Platform

CPU-first, Aerial-aligned engineering repository for an educational NR
PUSCH-oriented baseband vertical slice, FAPI-like control plane, fronthaul/RU
emulator, MIMO/scheduler baselines, and reproducible CPU/GPU benchmark harnesses.

**Not a claim of 3GPP conformance or carrier-grade status.**

## Host notes

- Apple M2 / CPU-only hosts: configure, build, and test the **CPU** path.
- CUDA targets are optional; missing GPU evidence is labeled `BLOCKED_HARDWARE`.
- Never fabricate GPU, NIC, PTP, or SDR measurements.

## Quick start (CPU)

```bash
cmake --preset cpu
cmake --build --preset cpu
ctest --preset cpu --output-on-failure
make smoke
```

Or: `make gate4-cpu`

## Layout

| Path | Role |
|------|------|
| `include/nr_bb`, `src/` | PHY vertical slice + MIMO/scheduler/FAPI/fronthaul |
| `cuda/` | CUDA candidate kernels (build only if CUDA available) |
| `tests/`, `fuzz/`, `vectors/` | Unit tests, fuzz corpus, golden vectors |
| `benchmarks/` | Performance harness → JSON under `results/benchmarks/` |
| `docs/lab/` | Gate 6 lab packets |
| `paper/` | Benchmark paper draft |
| `artifact/` | Release candidate staging |
| `upstream/` | DOCUMENTED_IMPLEMENTATION contribution packets |

## Status vocabulary

`PASS` · `FAIL` · `BLOCKED_HARDWARE` · `PENDING_LAB` · `DOCUMENTED_IMPLEMENTATION` · `DOI_PENDING` · `RELEASE_CANDIDATE_READY`

## License

MIT — see `LICENSE`. Cite via `CITATION.cff`.
