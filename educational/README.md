# Educational substitutes (NOT on acceptance path)

These modules are retained for pedagogy and historical benchmarks. The Gate 4 /
Track C CPU acceptance path **must not** depend solely on:

| Educational artifact | Acceptance replacement |
|----------------------|------------------------|
| (16,8) short LDPC + bit-flipping | Real NR QC-LDPC BG1/BG2 + min-sum |
| **CompactQcLdpc** (`nr_style_compact_ldpc/`) | Real TS 38.212 BG tables (`ldpc_bg_tables.hpp`) |
| Naive O(N²) DFT OFDM | Radix-2 FFT OFDM |
| Binary PAM / Gray-ish QAM | Exact Gray QPSK/16/64/256QAM + max-log LLR |
| Constant-magnitude hard LLR | Soft max-log-MAP LLR |
| CUDA AXPY-only kernels | Baseband modulate/demod/scramble/RM/ZF/MMSE/HARQ kernels |
| PF-ratio-only scheduler | UE filter, metrics, PRB, MCS, QoS, HARQ, replay |

Label: `EDUCATIONAL_ONLY` — see `docs/reference/LDPC_SOURCE_AND_LICENSE_AUDIT.md`.
