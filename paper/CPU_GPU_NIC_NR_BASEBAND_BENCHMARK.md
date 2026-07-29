# CPU / GPU / NIC NR Baseband Benchmark

**Status:** draft research artifact · **Not** 3GPP conformance · **Not** carrier-grade

## 0. Path separation (corrective depth)

| Path | What it is | Gate / paper use |
|------|------------|------------------|
| **Educational** (`educational/`) | (16,8) LDPC, naive DFT, binary PAM, constant hard LLR, AXPY toy, PF-ratio-only notes | Pedagogy / ablations only — **not** sole acceptance |
| **Standards-path CPU** (`include/nr_bb`, `src/`) | CRC24A/B, Gold scramble, Gray QPSK/16/64/256QAM + max-log LLR, radix-2 FFT OFDM, NR QC-LDPC BG1/BG2 **lifting scaffolds**, RM recovery scaffolding, ZF/MMSE tones, MacScheduler, FAPI v1 SM, BER hooks, numerology deadlines | Gate 4 CPU reference / unit tests |
| **Pending GPU / NIC / SDR** | CUDA baseband kernels + Nsight / PTP / RU lab | `BLOCKED_HARDWARE` or `PENDING_LAB` on Apple M2 — **never invent timings** |

## 1. Architecture (standards-path)

Transport bits → CRC → segmentation → **NR QC-LDPC scaffold (BG1/BG2 lift + min-sum)** →
rate matching/recovery → Gold scrambling → **Gray QAM + max-log LLR** → layer mapping →
DMRS → **FFT OFDM** → channel → LS/ZF/MMSE est → HARQ soft buffer → BER campaign hooks.

Control: versioned FAPI-like state machine + fixtures/fuzz. Fronthaul: C/U/S metadata + IQ
replay (no RF TX). Scheduler: UE filter, composite metrics, PRB, MCS, QoS, HARQ, replay hash.

## 2. Spec traceability

| Block | Spec touchpoint | Claim level |
|-------|-----------------|-------------|
| CRC24A/B | TS 38.212 §5.1 | poly/width aligned; bit-serial educational encode |
| LDPC | TS 38.212 §5.3.2 | **BG1/BG2 lifting mechanics** on compact scaffolds; not full-table bit-exact |
| Rate matching | TS 38.212 §5.4.2 | circular-buffer RV scaffolding |
| Scrambling | TS 38.211 §5.2.1 | Gold LFSR |
| Modulation | TS 38.211 §5.1.3 | Gray + unit-energy + max-log-MAP soft |
| OFDM | TS 38.211 §5.3 | radix-2 FFT + CP (configurable) |
| FAPI-like | SCF FAPI families | versioned research SM |
| Fronthaul | eCPRI / O-RAN FH concepts | metadata + IQ only |

Educational (16,8) LDPC / naive DFT remain under `educational/` with `EDUCATIONAL_ONLY` labels.

## 3. Correctness method

- Catch2 unit tests (constellation energy, hard roundtrip, soft under noise, LDPC syndrome,
  FFT roundtrip, scheduler replay, FAPI versioning, CUDA CPU refs).
- Self-generated vectors (`vectors/`) + `TOLERANCES.md`.
- FAPI sequencing + corruption fuzz (`fuzz/fuzz_fapi.cpp`).
- ≥6 controlled **CPU** optimization studies (`make optimization-study-cpu`).

## 4. Profiles

| Profile | Host | Status |
|---------|------|--------|
| CPU Release Ninja | Apple M2 | PASS (author) when tests green |
| CPU ASan/UBSan | Apple M2 | presets via `make sanitizers` |
| CUDA baseband kernels | NVIDIA lab | `BLOCKED_HARDWARE` on author Mac |
| NIC/PTP / SDR-RU | lab | `PENDING_LAB` / `gate6-dry-run` FAIL-CLOSED |

## 5. Optimizations (≥6 controlled CPU studies)

Controlled one-variable before/after studies with hypothesis, percentiles, conclusion:

1. scalar vs unrolled accumulate  
2. naive DFT vs radix-2 FFT  
3. heap alloc vs pool reuse  
4. AoS vs SoA  
5. pageable vs aligned hot buffer (**host-side only**)  
6. sync vs async **CPU stub** (not fake CUDA streams)

GPU study slot emits `BLOCKED_HARDWARE` pending JSON — no fabricated numbers.

## 6. Limitations / evidence labels

- Compact BG1/BG2 scaffolds are research-sized; full 3GPP edge tables not claimed bit-exact.
- CUDA `.cu` sources are buildable where `nvcc` exists; numeric GPU evidence is
  `BLOCKED_HARDWARE` here.
- Gate 6 `make gate6-dry-run` is **FAIL-CLOSED** (`physical_pass: false`).
- Upstream srsRAN/OAI patches: `DOCUMENTED_IMPLEMENTATION` until merge.

## 7. Reproduce

```bash
make bootstrap test reference-vectors
make gate4-cpu-reference optimization-study-cpu orchestrator gate6-dry-run
make gate4-cuda-build   # → BLOCKED_HARDWARE JSON on Mac
make paper artifact
```

## References

1. 3GPP TS 38.212 — Multiplexing and channel coding (NR).
2. 3GPP TS 38.211 — Physical channels and modulation (NR).
3. srsRAN Project documentation (open-source NR reference orientation).
4. OpenAirInterface 5G documentation (open-source PHY/MAC orientation).
5. NVIDIA Aerial SDK overview materials (public documentation only; no proprietary code used).

