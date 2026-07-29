#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
DEST=artifact/release
mkdir -p "$DEST"
cp -f README.md LICENSE CITATION.cff codemeta.json "$DEST/" 2>/dev/null || true
cp -f paper/CPU_GPU_NIC_NR_BASEBAND_BENCHMARK.md "$DEST/" 2>/dev/null || true
mkdir -p "$DEST/results" "$DEST/vectors" "$DEST/configs"
cp -R results/benchmarks "$DEST/results/" 2>/dev/null || true
cp -R results/profiles "$DEST/results/" 2>/dev/null || true
cp -R vectors/* "$DEST/vectors/" 2>/dev/null || true
cat > "$DEST/STATUS.md" << 'S'
# Artifact status

- RELEASE_CANDIDATE_READY: yes (CPU path)
- DOI_PENDING: yes
- GPU evidence: BLOCKED_HARDWARE on author Mac host
- NIC/PTP evidence: PENDING_LAB
- 3GPP conformance: NOT CLAIMED
S
echo "Artifact staged at $DEST"
