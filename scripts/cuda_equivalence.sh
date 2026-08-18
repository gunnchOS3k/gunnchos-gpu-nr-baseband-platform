#!/usr/bin/env bash
# CUDA correctness / CPU↔GPU equivalence harness.
# Without nvcc / NVIDIA GPU: emit BLOCKED_GPU (never invent GPU timings).
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
OUT="$ROOT/results/cuda_equivalence"
mkdir -p "$OUT" "$ROOT/results/blocked_gpu"
chmod +x "$ROOT/scripts/emit_blocked_gpu.sh"

has_nvcc=0
has_gpu=0
if command -v nvcc >/dev/null 2>&1; then has_nvcc=1; fi
if command -v nvidia-smi >/dev/null 2>&1 && nvidia-smi -L >/dev/null 2>&1; then has_gpu=1; fi

emit_blocked() {
  local name="$1" note="$2"
  cat >"$OUT/${name}.json" <<EOF
{
  "harness": "$name",
  "status": "BLOCKED_GPU",
  "aliases": ["BLOCKED_HARDWARE"],
  "fail_closed": true,
  "gpu_timings_present": false,
  "numeric_gpu_claim": false,
  "notes": "$note"
}
EOF
}
"$ROOT/scripts/emit_blocked_gpu.sh" "$ROOT/results/blocked_gpu/BLOCKED_GPU.json" cuda-equivalence 2>/dev/null || true

if [[ "$has_nvcc" -eq 0 || "$has_gpu" -eq 0 ]]; then
  emit_blocked "cuda_correctness" "No nvcc and/or no NVIDIA GPU on this host (Apple M2)"
  emit_blocked "cuda_equivalence" "CPU reference exists; GPU numeric equivalence BLOCKED_HARDWARE"
  # Still exercise CPU reference self-consistency when binary is present.
  if [[ -x "$ROOT/build/cpu/nr_bb_tests" ]]; then
    "$ROOT/build/cpu/nr_bb_tests" "[cuda_ref]" || true
  elif [[ -x "$ROOT/build/nr_bb_tests" ]]; then
    "$ROOT/build/nr_bb_tests" "[cuda_ref]" || true
  fi
  echo "cuda-correctness / cuda-equivalence → BLOCKED_GPU"
  cat "$OUT/cuda_correctness.json"
  exit 0
fi

# Lab path (self-hosted nvidia-gpu runner): build CUDA and compare — not executed on Mac.
echo "GPU path reserved for self-hosted nvidia-gpu runner"
emit_blocked "cuda_equivalence" "Unexpected: nvcc+GPU present but lab compare script not invoked in this stub"
exit 0
