# CUDA baseband candidates

| File | Kernels | Host status |
|------|---------|-------------|
| `baseband_kernels.cu` | scramble, rate-match, QPSK mod/LLR, ZF/MMSE tones, LS est, HARQ combine, scheduler metric | build needs `nvcc`; numeric `BLOCKED_HARDWARE` on Apple M2 |
| `scheduler_candidate.cu` | PF-style metric toy | same |
| `benchmark_kernels.cu` | timing harness hooks | same |

CPU equivalence: `nr_bb::cuda_ref::*` + `make cuda-correctness cuda-equivalence`.

cuFFT OFDM is a **lab** candidate (document in Gate 6 / self-hosted runner); do not invent
GPU FFT timings on CPU-only hosts.
