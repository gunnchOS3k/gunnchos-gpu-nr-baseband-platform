#!/usr/bin/env bash
set -euo pipefail
OUTDIR="${1:-results/profiles}"
mkdir -p "$OUTDIR"
STATUS="BLOCKED_HARDWARE"
NOTE="Nsight Systems/Compute unavailable without NVIDIA GPU"
if command -v nsys >/dev/null 2>&1; then
  STATUS="PENDING_LAB"
  NOTE="nsys found but lab run not executed in this script path"
fi
if command -v ncu >/dev/null 2>&1; then
  STATUS="PENDING_LAB"
fi
cat > "$OUTDIR/nsight_systems.json" << JSON
{"tool":"nsys","status":"$STATUS","notes":"$NOTE","timestamp":"$(date -u +%Y-%m-%dT%H:%M:%SZ)"}
JSON
cat > "$OUTDIR/nsight_compute.json" << JSON
{"tool":"ncu","status":"$STATUS","notes":"$NOTE","timestamp":"$(date -u +%Y-%m-%dT%H:%M:%SZ)"}
JSON
# host perf / sanitizer placeholders
if command -v perf >/dev/null 2>&1; then
  echo '{"tool":"perf","status":"PENDING_LAB"}' > "$OUTDIR/perf.json"
else
  echo '{"tool":"perf","status":"BLOCKED_HARDWARE","notes":"perf not available on this host"}' > "$OUTDIR/perf.json"
fi
echo '{"tool":"asan_ubsan","status":"DOCUMENTED_IMPLEMENTATION","presets":["cpu-asan","cpu-ubsan"]}' > "$OUTDIR/sanitizer.json"
echo "GPU profile fixtures written with status=$STATUS"
