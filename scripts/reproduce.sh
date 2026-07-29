#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
./scripts/gate4_cpu.sh
./scripts/gate4_gpu.sh
./scripts/system_manifest.sh
echo "REPRODUCE complete (GPU may be BLOCKED_HARDWARE)"
