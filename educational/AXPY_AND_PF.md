# Educational CUDA AXPY + PF-ratio-only notes

**Status:** `EDUCATIONAL_ONLY` / superseded on acceptance path.

- Historical CUDA candidate used a lone `axpy` kernel (`cuda/benchmark_kernels.cu`
  kept as a microbench toy). Acceptance CUDA path uses baseband kernels in
  `cuda/baseband_kernels.cu` (modulate / demod LLR / scramble / rate-match stubs)
  with CPU reference comparison schema. On Apple M2: `BLOCKED_HARDWARE` for GPU
  numeric results — never invent timings.
- Historical scheduler ranked UEs by PF ratio alone. Acceptance scheduler adds
  UE filtering, metric vector, PRB packing, MCS/QoS/HARQ state, and deterministic
  replay (`nr_bb::MacScheduler`).
