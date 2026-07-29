# CUDA candidate sources

These `.cu` files compile only when `NR_BB_ENABLE_CUDA=ON` and a CUDA toolkit
is available. On Apple Silicon / CPU-only hosts:

- Do **not** fabricate GPU timings.
- Emit fixtures with status `BLOCKED_HARDWARE`.
- Use `make gate4-gpu` / `scripts/profile_gpu.sh` which detect absence.

Status vocabulary: `PASS`, `FAIL`, `BLOCKED_HARDWARE`, `PENDING_LAB`, `DOCUMENTED_IMPLEMENTATION`.
