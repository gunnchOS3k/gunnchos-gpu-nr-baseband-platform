# Claim boundaries — GPU NR baseband (research extension)

This repository is a **PHY/baseband implementation capability**, not a dissertation paper and **not** a 3GPP-conformance product.

Six evidence classes must stay distinct. A PASS in one class never promotes another.

| Class | What it is | What a PASS means | What it is not |
|---|---|---|---|
| **educational substitute** | `educational/` short LDPC, CompactQcLdpc, naive DFT, binary PAM, AXPY kernels | Pedagogy / historical benchmark ran | NR acceptance; Gate 4 Track C |
| **standards-path** | `src/` + `include/nr_bb/` CRC, BG1/BG2 LDPC, RM, scramble, Gray QAM, OFDM FFT, eq, HARQ, FAPI | Code exists and CPU tests exercise it | Certified 3GPP implementation |
| **CPU validation** | Catch2 tests, golden vectors, CPU benches, sanitizers, PHY adapter reports | Deterministic CPU behaviour matched vectors / self-consistency | GPU measured; lab RF |
| **CUDA candidate** | `cuda/*.cu` kernels + `nr_bb::cuda_ref` CPU twins | Source compiles **or** is present as a candidate | Measured GPU timing |
| **GPU measured** | Nsight / `nvidia-smi` / self-hosted `nvidia-gpu` runner numeric JSON | Hardware + procedure + timings from that GPU | Invented Mac timings |
| **3GPP conformance** | Independent RAN4 / test-equipment campaign | Not in this repo | Never claimed |

## Status vocabulary

`PASS` · `FAIL` · `BLOCKED_HARDWARE` · `BLOCKED_GPU` · `BLOCKED_GPU_RUNNER` · `PENDING_LAB` · `EDUCATIONAL_ONLY` · `DOCUMENTED_IMPLEMENTATION` · `REFERENCE_DEPENDENCY_UNAVAILABLE` · `COMPARISON_PASS` / `COMPARISON_FAIL`

On Apple Silicon without NVIDIA GPU, CUDA/GPU targets **must** emit `BLOCKED_GPU` JSON with `gpu_timings_present: false`. Never invent kernel times.

## Dissertation role

Optional engineering extension under 6G Flagship **Devices and Circuit Technology** (public theme mapping only). **Not** a fourth paper. No University of Oulu appointment is claimed.
