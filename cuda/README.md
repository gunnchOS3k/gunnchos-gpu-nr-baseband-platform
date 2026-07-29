# CUDA candidates

| File | Role | Mac host |
|------|------|----------|
| `baseband_kernels.cu` | scramble / rate-match / QPSK mod / QPSK LLR | build blocked without nvcc; numeric `BLOCKED_HARDWARE` |
| `scheduler_candidate.cu` | metric ranking candidate | same |
| `benchmark_kernels.cu` | EDUCATIONAL AXPY microbench only | not acceptance |

CPU references live in `nr_bb::cuda_ref`. Never invent GPU timings.
