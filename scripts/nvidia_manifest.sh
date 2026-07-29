#!/usr/bin/env bash
set -euo pipefail
OUT="${1:-results/profiles/nvidia_gpu_manifest.json}"
mkdir -p "$(dirname "$OUT")"
if command -v nvidia-smi >/dev/null 2>&1; then
  nvidia-smi -q > results/profiles/nvidia-smi.txt || true
  STATUS="PASS"
  NOTE="nvidia-smi available"
else
  STATUS="BLOCKED_HARDWARE"
  NOTE="nvidia-smi not found; Apple M2 / CPU-only host"
fi
if command -v nvcc >/dev/null 2>&1; then
  NVCC=$(nvcc --version | tail -1)
else
  NVCC="BLOCKED_HARDWARE"
fi
cat > "$OUT" << JSON
{
  "status": "$STATUS",
  "nvidia_smi": "$STATUS",
  "nvcc": "$NVCC",
  "notes": "$NOTE",
  "timestamp": "$(date -u +%Y-%m-%dT%H:%M:%SZ)"
}
JSON
echo "Wrote $OUT ($STATUS)"
