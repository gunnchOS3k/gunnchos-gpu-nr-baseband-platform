# Controlled optimization study schema

Each study JSON under `results/optimization_studies/` MUST include:

| Field | Required | Notes |
|-------|----------|-------|
| `study_id` | yes | Stable id |
| `controlled_variable` | yes | Single variable under test |
| `platform` | yes | `cpu` or `gpu` |
| `timing_source` | yes | `cpu_synthetic` / `nsight` / `blocked` |
| `gpu_claim` | yes | `none` or explicit; never invent on Mac |
| `hypothesis` | yes | One sentence |
| `before` / `after` | yes for CPU | `{name, median_us, p90_us, p95_us, p99_us, max_us}` |
| `conclusion` | yes | One sentence |
| `status` | yes | `PASS` or `BLOCKED_HARDWARE` |

## CPU studies (executed by `make optimization-study-cpu`)

1. scalar vs SIMD-friendly unroll  
2. naive DFT vs radix-2 FFT  
3. heap alloc vs pool reuse  
4. AoS vs SoA  
5. pageable vs aligned hot buffer (host-side only)  
6. sync vs async CPU staging stub  

## GPU studies

`make optimization-study-gpu` → `BLOCKED_HARDWARE` on Apple M2 / no NVIDIA GPU.
**Never fabricate GPU timings.**
