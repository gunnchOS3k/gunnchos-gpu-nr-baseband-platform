#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
cmake --preset cpu
cmake --build --preset cpu
ctest --preset cpu --output-on-failure
cmake --build --preset cpu --target nr_bb_bench
./build/cpu/nr_bb_bench
./scripts/system_manifest.sh
echo "gate4-cpu-reference complete"
