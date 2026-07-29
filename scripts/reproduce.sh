#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
make bootstrap test
make optimization-study-cpu orchestrator gate6-dry-run
make gate4-cuda-build
echo "reproduce.sh complete (GPU numeric BLOCKED_HARDWARE expected on Mac)"
