#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
./scripts/nvidia_manifest.sh
./scripts/profile_gpu.sh
if ! command -v nvidia-smi >/dev/null 2>&1; then
  mkdir -p results/benchmarks
  cat > results/benchmarks/gate4_gpu.json << JSON
{"gate":"gate4-gpu","status":"BLOCKED_HARDWARE","notes":"No NVIDIA GPU on host","timestamp":"$(date -u +%Y-%m-%dT%H:%M:%SZ)"}
JSON
  echo "GATE4 GPU BLOCKED_HARDWARE"
  exit 0
fi
cmake --preset gpu
cmake --build --preset gpu
echo "GATE4 GPU built"
