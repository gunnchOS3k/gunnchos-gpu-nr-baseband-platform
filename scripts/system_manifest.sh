#!/usr/bin/env bash
set -euo pipefail
OUT="${1:-results/profiles/system_manifest.json}"
mkdir -p "$(dirname "$OUT")"
OS=$(uname -s)
ARCH=$(uname -m)
CPU=$(sysctl -n machdep.cpu.brand_string 2>/dev/null || uname -p)
NCPU=$(sysctl -n hw.ncpu 2>/dev/null || getconf _NPROCESSORS_ONLN)
MEM=$(sysctl -n hw.memsize 2>/dev/null || echo 0)
CMAKE_V=$(cmake --version 2>/dev/null | head -1 || echo missing)
CXX_V=$(clang++ --version 2>/dev/null | head -1 || echo missing)
cat > "$OUT" << JSON
{
  "status": "PASS",
  "os": "$OS",
  "arch": "$ARCH",
  "cpu": "$CPU",
  "ncpu": $NCPU,
  "mem_bytes": $MEM,
  "cmake": "$CMAKE_V",
  "cxx": "$CXX_V",
  "timestamp": "$(date -u +%Y-%m-%dT%H:%M:%SZ)"
}
JSON
echo "Wrote $OUT"
