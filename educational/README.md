# Educational substitutes (NOT on acceptance path)

These modules are retained for pedagogy and historical benchmarks. The Gate 4
CPU acceptance path (`make gate4-cpu-reference`, unit tests under `tests/`)
**must not** depend solely on:

| Educational artifact | Acceptance replacement |
|----------------------|------------------------|
| (16,8) short LDPC + bit-flipping | NR QC-LDPC BG1/BG2 lifting + min-sum |
| Naive O(N²) DFT OFDM | Radix-2 FFT OFDM |
| Binary PAM / Gray-ish QAM | Exact Gray QPSK/16/64/256QAM + max-log LLR |
| Constant-magnitude hard LLR | Soft max-log-MAP LLR |
| CUDA AXPY-only kernels | Baseband modulate/demod/scramble/RM kernels (CPU ref + blocked GPU) |
| PF-ratio-only scheduler | UE filter, metrics, PRB, MCS, QoS, HARQ, replay |

Label: `EDUCATIONAL_ONLY` — never cite these as 3GPP conformance or Gate pass criteria.
