#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
./scripts/nvidia_manifest.sh
./scripts/profile_gpu.sh
mkdir -p results/benchmarks
if ! command -v nvidia-smi >/dev/null 2>&1 || ! command -v nvcc >/dev/null 2>&1; then
  ./scripts/emit_pending_json.sh BLOCKED_HARDWARE gate4-cuda-build \
    results/benchmarks/gate4_cuda_build.json \
    "No nvcc/nvidia-smi — CUDA static build blocked on this host"
  ./scripts/emit_pending_json.sh BLOCKED_HARDWARE gate4-gpu \
    results/benchmarks/gate4_gpu.json \
    "No NVIDIA GPU — never invent GPU timings"
  echo "GATE4 CUDA/GPU BLOCKED_HARDWARE"
  exit 0
fi
cmake --preset gpu
cmake --build --preset gpu
./scripts/emit_pending_json.sh PENDING_LAB gate4-gpu \
  results/benchmarks/gate4_gpu.json \
  "CUDA built; lab numeric evidence still required"
echo "GATE4 GPU built"
