# Reproducibility — gunnchos GPU NR baseband platform

CPU-first NR PUSCH-oriented vertical slice. GPU numbers are produced only on an NVIDIA host.

## Fresh machine (CPU)

```bash
git clone https://github.com/gunnchOS3k/gunnchos-gpu-nr-baseband-platform.git
cd gunnchos-gpu-nr-baseband-platform
# Private clone requires collaborator access (see docs/packets/REPOSITORY_VISIBILITY_PACKET.md).
cmake --preset cpu
cmake --build --preset cpu
ctest --preset cpu --output-on-failure
make validate-phy-independent
make cuda-correctness   # expected BLOCKED_GPU on hosts without NVIDIA GPU
make supervisor-cpu-gate
```

Requires: CMake ≥ 3.20, Ninja, C++20 compiler, Python 3.10+. Catch2 is fetched at configure time.

## Expected CPU outputs

- Catch2 tests PASS (educational + standards-path).
- `results/phy_independent/summary.json` is `PASS` or honest `PENDING` if reference adapters are unavailable.
- `results/blocked_gpu/BLOCKED_GPU.json` with `status: BLOCKED_GPU` when `nvcc`/`nvidia-smi` are absent.
- CPU benches under `results/benchmarks/` labeled CPU (not GPU).

## GPU path (lab only)

```bash
make gate4-gpu
make cuda-equivalence
```

If CUDA hardware is missing, scripts exit 0 after writing fail-closed JSON. They must not fabricate timings.

## Evidence labels

| Artifact | Class |
|---|---|
| `educational/` tests | `EDUCATIONAL_ONLY` |
| `vectors/golden_*.txt` CPU match | `CPU validation` |
| `cuda/*.cu` without Nsight JSON | `CUDA candidate` |
| Nsight / `nvidia-smi` timings | `GPU measured` |
| This repo as a whole | **not** 3GPP conformance |

Record `git rev-parse HEAD` in any supervisor packet. PHY vector map: `docs/PHY_VECTOR_TRACEABILITY.md`.
