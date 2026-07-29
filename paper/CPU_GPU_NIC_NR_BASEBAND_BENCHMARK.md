# CPU / GPU / NIC NR Baseband Benchmark

**Status:** draft research artifact · **Not** 3GPP conformance · **Not** carrier-grade  
**Track C:** real NR BG1/BG2 depth (educational vs standards-path vs pending GPU)

## 0. Path separation

| Path | What it is | Gate / paper use |
|------|------------|------------------|
| **Educational** (`educational/`, `educational/nr_style_compact_ldpc/`) | (16,8) LDPC, **CompactQcLdpc** scaffolds, naive DFT, binary PAM | Pedagogy only — **EDUCATIONAL_ONLY**, not NR acceptance |
| **Standards-path CPU** (`include/nr_bb`, `src/`) | CRC24A/B, Gold scramble, Gray QAM + max-log LLR, FFT OFDM, **real NR QC-LDPC BG1/BG2** (TS 38.212 tables via Apache-2.0 Sionna CSV), segmentation + CRC24B + filler, RM RVs, ZF/MMSE, HARQ soft combine, FAPI SM | Gate 4 / Track C CPU acceptance |
| **Pending GPU / NIC / SDR** | CUDA scramble/RM/QAM/LLR/ZF/MMSE/HARQ/scheduler (+ cuFFT lab) | `BLOCKED_HARDWARE` / `BLOCKED_GPU_RUNNER` on Apple M2 — **never invent timings** |

License audit: `docs/reference/LDPC_SOURCE_AND_LICENSE_AUDIT.md`.

## 1. Architecture (standards-path)

Transport bits → CRC → segmentation (**CRC24B** when C>1, filler to kb·Zc) →
**NR QC-LDPC BG1/BG2** (real base graphs + lifting Table 5.3.2-1 + min-sum) →
rate matching/recovery (RV k0 coeffs) → Gold scrambling → Gray QAM + max-log LLR →
layer mapping → DMRS → FFT OFDM → channel → LS/ZF/MMSE → HARQ soft buffer.

## 2. Spec traceability

| Block | Spec touchpoint | Claim level |
|-------|-----------------|-------------|
| CRC24A/B | TS 38.212 §5.1 | poly/width aligned |
| LDPC | TS 38.212 §5.3.2 | **Real BG1/BG2 tables** (Sionna Apache-2.0 CSV provenance); encode+min-sum on CPU |
| Segmentation | TS 38.212 §5.2.2 | C, CRC24B, filler, Zc/i_ls selection |
| Rate matching | TS 38.212 §5.4.2 | circular buffer + Table 5.4.2.1-2 RV coeffs; 2Z puncture |
| Scrambling | TS 38.211 §5.2.1 | Gold LFSR |
| Modulation | TS 38.211 §5.1.3 | Gray + max-log-MAP soft |
| OFDM | TS 38.211 §5.3 | radix-2 FFT + CP |

CompactQcLdpc remains under `educational/nr_style_compact_ldpc/` and is **not** acceptance.

## 3. Independent PHY validation

Adapters: `tools/reference_adapters/{sionna,srsran,oai,matlab}/`.  
`make validate-phy-independent` requires ≥2 `COMPARISON_PASS` for overall PASS.  
On this author host (no Sionna/srsRAN/OAI/MATLAB): honest `REFERENCE_DEPENDENCY_UNAVAILABLE` → **PENDING**.

## 4. Correctness method

- Catch2: BG selection, Zc set, noiseless LDPC roundtrip, filler, RM RVs, HARQ combine, CUDA CPU refs.
- Self-generated vectors + `TOLERANCES.md`.
- `make cuda-correctness cuda-equivalence` → `BLOCKED_HARDWARE` without GPU.
- ≥6 controlled **CPU** optimization studies (`docs/optimization/STUDY_SCHEMA.md`).

## 5. Profiles

| Profile | Host | Status |
|---------|------|--------|
| CPU Release | Apple M2 | PASS when tests green |
| CUDA / self-hosted `nvidia-gpu` | lab runner | `BLOCKED_HARDWARE` / `BLOCKED_GPU_RUNNER` here |
| NIC/PTP / SDR-RU | lab | `PENDING_LAB` / Gate 6 FAIL-CLOSED |

## 6. Limitations / evidence labels

- Real BG tables are integrated; full MCS×Zc matrix bit-exact vs commercial stacks not claimed.
- GPU numeric evidence is `BLOCKED_HARDWARE` on author Mac.
- Independent PHY PASS needs external deps (pending on this host).

## 7. Reproduce

```bash
make bootstrap test reference-vectors
make validate-phy-independent
make cuda-correctness cuda-equivalence   # BLOCKED_HARDWARE on Mac
make gate4-cpu-reference optimization-study-cpu orchestrator gate6-dry-run
make paper artifact
```

## References

1. 3GPP TS 38.212 — Multiplexing and channel coding (NR).
2. 3GPP TS 38.211 — Physical channels and modulation (NR).
3. NVIDIA Sionna (Apache-2.0) — LDPC BG CSV provenance only; no Aerial/cuBB code.
4. srsRAN Project / OpenAirInterface — adapter targets (`DOCUMENTED` / dependency-gated).
5. NVIDIA Aerial SDK public docs only — GPU lab pending.
