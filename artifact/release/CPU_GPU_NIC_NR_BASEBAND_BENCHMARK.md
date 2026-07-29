# CPU / GPU / NIC NR Baseband Benchmark

**Status:** draft research artifact paper · **Not** 3GPP conformance · **Not** carrier-grade claim

## 1. Architecture

This artifact implements an educational NR PUSCH-oriented vertical slice on CPU
with optional CUDA candidate kernels:

Transport bits → CRC → segmentation → LDPC (short educational) → rate matching →
scrambling → modulation → layer mapping → DMRS → OFDM → channel (CFO/timing) →
channel estimation → ZF/MMSE/RZF equalization → LLR → HARQ soft buffer → BER/BLER.

Control/data plane: bounded FAPI-like state machine. Fronthaul: C/U/S conceptual
planes with eCPRI/O-RAN-style metadata and IQ replay (no RF TX).

## 2. Spec traceability (educational)

| Block | Spec touchpoint | Claim level |
|-------|-----------------|-------------|
| CRC24A/B | TS 38.212 §5.1 | educational poly / width |
| Segmentation | TS 38.212 §5.2.1 | simplified CB size |
| LDPC | TS 38.212 §5.3.2 | **not** BG1/BG2; short (16,8) |
| Rate matching | TS 38.212 §5.4.2 | puncture/repeat only |
| Scrambling | TS 38.211 §5.2.1 / §6.3.1.1 | Gold LFSR educational |
| Modulation | TS 38.211 §5.1 | Gray-ish unit-energy |
| DMRS / OFDM | TS 38.211 §6.4.1.1 / §5.3 | simplified |
| FAPI-like | SCF FAPI families | bounded research SM |
| Fronthaul | eCPRI / O-RAN FH concepts | metadata + IQ only |

## 3. Correctness method

- Unit tests (Catch2) per module with error-path checks.
- Self-generated golden vectors (`vectors/`) with documented tolerances.
- Optional external adapters (Sionna/srsRAN/OAI/pyAerial) are stubs only.
- FAPI sequencing + corruption fuzz (`fuzz/fuzz_fapi.cpp`).

## 4. Profiles

| Profile | Host | Status |
|---------|------|--------|
| CPU Release Ninja | Apple M2 | PASS (author) |
| CPU ASan/UBSan | Apple M2 | DOCUMENTED_IMPLEMENTATION presets |
| CUDA graphs/streams | NVIDIA lab | BLOCKED_HARDWARE on author Mac |
| NIC/PTP | lab | PENDING_LAB |

## 5. Optimizations under test (≥6 comparison slots)

| # | Slot | Status | Evidence |
|---|------|--------|----------|
| 1 | CRC24A CPU bit-serial | PASS | `results/benchmarks/cpu_crc24a.json` |
| 2 | Educational LDPC (16,8) | PASS | `results/benchmarks/cpu_ldpc_short.json` |
| 3 | OFDM DFT 64 | PASS | `results/benchmarks/cpu_ofdm_64.json` |
| 4 | MMSE 4×4 equalizer | PASS | `results/benchmarks/cpu_eq_mmse_4x4.json` |
| 5 | PF scheduler CPU | PASS | `results/benchmarks/cpu_scheduler_pf.json` |
| 6 | Memory layout pageable/pinned mock | PASS | `results/benchmarks/cpu_memory_layout.json` |
| 7 | CUDA graphs/streams | BLOCKED_HARDWARE | `results/benchmarks/cuda_graphs_streams.json` |
| 8 | NIC timestamp / PTP path | PENDING_LAB | gate6 dry-run registry |

Fill GPU/NIC numeric cells only from instrumented lab registries — never fabricate.

## 6. Limitations

Educational LDPC and simplified RM/DMRS/OFDM are intentionally non-conforming.
Results are for engineering method evidence, not certification.
