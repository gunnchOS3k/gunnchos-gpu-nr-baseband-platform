#!/usr/bin/env bash
# Fail-closed GPU blocker. Never invent timings.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
OUT="${1:-$ROOT/results/blocked_gpu/BLOCKED_GPU.json}"
GATE="${2:-supervisor-cpu-gate}"
mkdir -p "$(dirname "$OUT")"

has_nvcc=0
has_gpu=0
if command -v nvcc >/dev/null 2>&1; then has_nvcc=1; fi
if command -v nvidia-smi >/dev/null 2>&1 && nvidia-smi -L >/dev/null 2>&1; then has_gpu=1; fi

host="$(uname -s)-$(uname -m)"
ts="$(date -u +%Y-%m-%dT%H:%M:%SZ)"
sha="$(git -C "$ROOT" rev-parse HEAD 2>/dev/null || echo UNCOMMITTED)"

if [[ "$has_nvcc" -eq 1 && "$has_gpu" -eq 1 ]]; then
  cat >"$OUT" <<JSON
{
  "gate": "$GATE",
  "status": "CUDA_HARDWARE_PRESENT",
  "aliases": [],
  "fail_closed": true,
  "gpu_timings_present": false,
  "numeric_gpu_claim": false,
  "nvcc": true,
  "nvidia_smi": true,
  "host": "$host",
  "commit": "$sha",
  "timestamp": "$ts",
  "notes": "Hardware present; timings still require Nsight / gate4-gpu numeric JSON — this file is not a GPU measured PASS"
}
JSON
  echo "Wrote $OUT (CUDA_HARDWARE_PRESENT, timings not claimed)"
  exit 0
fi

cat >"$OUT" <<JSON
{
  "gate": "$GATE",
  "status": "BLOCKED_GPU",
  "aliases": ["BLOCKED_HARDWARE", "BLOCKED_GPU_RUNNER"],
  "fail_closed": true,
  "gpu_timings_present": false,
  "numeric_gpu_claim": false,
  "nvcc": $has_nvcc,
  "nvidia_smi": $has_gpu,
  "host": "$host",
  "commit": "$sha",
  "timestamp": "$ts",
  "notes": "No nvcc and/or no NVIDIA GPU. CUDA candidate sources may exist; GPU measured evidence is absent. Never invent kernel timings."
}
JSON
echo "Wrote $OUT (BLOCKED_GPU)"
