# State — validation-gate machine

Promotion is explicit and fail-closed. No automatic jump from CPU PASS to GPU measured or 3GPP.

```mermaid
stateDiagram-v2
  [*] --> EducationalSubstitute
  EducationalSubstitute --> CpuStandardsPath: real BG tables + src/ not edu-only
  CpuStandardsPath --> CpuValidated: Catch2 + golden vectors PASS
  CpuValidated --> CudaCandidate: cuda/*.cu present / cpu_ref twins
  CudaCandidate --> GpuMeasured: nvcc + nvidia-smi + numeric JSON
  GpuMeasured --> TgpConform: independent RAN4 / test equipment
  EducationalSubstitute --> EducationalOnly: label EDUCATIONAL_ONLY
  CudaCandidate --> BlockedGpu: no NVIDIA GPU
  GpuMeasured --> BlockedGpu: runner missing timings
  TgpConform --> NotClaimed: this repo never auto-enters

  EducationalOnly: EDUCATIONAL_ONLY
  CpuValidated: CPU validation PASS
  CudaCandidate: CUDA candidate
  GpuMeasured: GPU measured
  TgpConform: 3GPP conformance
  BlockedGpu: BLOCKED_GPU JSON
  NotClaimed: NOT CLAIMED
```

`scripts/supervisor_cpu_gate.sh` records the reachable state on this host. Typical Apple Silicon: `CpuValidated` + `BlockedGpu`.
