#!/usr/bin/env bash
# Supervisor CPU gate: run every CPU test possible; emit honest BLOCKED_GPU.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
OUT="$ROOT/results/supervisor"
mkdir -p "$OUT" results/blocked_gpu results/benchmarks

python3 "$ROOT/scripts/validate_claim_boundaries.py"

if [[ -x "$ROOT/build/cpu/nr_bb_tests" ]]; then
  echo "Using existing build/cpu/nr_bb_tests"
else
  cmake --preset cpu
  cmake --build --preset cpu
fi

ctest --preset cpu --output-on-failure
cmake --build --preset cpu --target nr_bb_bench nr_bb_opt_study nr_bb_orchestrator nr_bb_fuzz_fapi
"$ROOT/build/cpu/nr_bb_bench"
"$ROOT/build/cpu/nr_bb_opt_study"
"$ROOT/build/cpu/nr_bb_orchestrator"
"$ROOT/build/cpu/nr_bb_fuzz_fapi"

python3 "$ROOT/scripts/validate_phy_independent.py"
chmod +x "$ROOT/scripts/cuda_equivalence.sh" "$ROOT/scripts/emit_blocked_gpu.sh" "$ROOT/scripts/gate4_gpu.sh"
"$ROOT/scripts/cuda_equivalence.sh"
"$ROOT/scripts/gate4_gpu.sh"
"$ROOT/scripts/emit_blocked_gpu.sh" "$OUT/../blocked_gpu/BLOCKED_GPU.json" supervisor-cpu-gate
"$ROOT/scripts/ensure_gate6_report.sh"

python3 - <<'PY'
import hashlib, json, os, subprocess
from datetime import datetime, timezone
from pathlib import Path
root = Path(".").resolve()
vec_dir = root / "vectors"
hashes = {}
for p in sorted(vec_dir.rglob("*")):
    if p.is_file():
        hashes[str(p.relative_to(root))] = hashlib.sha256(p.read_bytes()).hexdigest()
(root / "results" / "supervisor" / "phy_vector_hashes.json").write_text(
    json.dumps({"files": hashes, "timestamp": datetime.now(timezone.utc).isoformat()}, indent=2) + "\n"
)
blocked = json.loads((root / "results" / "blocked_gpu" / "BLOCKED_GPU.json").read_text())
phy = {}
psum = root / "results" / "phy_independent" / "summary.json"
if psum.exists():
    phy = json.loads(psum.read_text())
try:
    sha = subprocess.check_output(["git", "rev-parse", "HEAD"], text=True).strip()
except Exception:
    sha = "UNCOMMITTED"
summary = {
    "gate": "supervisor-cpu-gate",
    "status": "CPU_PASS",
    "evidence_class": "CPU validation",
    "educational_separated": True,
    "standards_path_tested": True,
    "gpu": blocked,
    "phy_independent": {"status": phy.get("status"), "notes": phy.get("notes")},
    "commit": sha,
    "host": f"{os.uname().sysname}-{os.uname().machine}",
    "timestamp": datetime.now(timezone.utc).isoformat(),
    "claim_3gpp_conformance": False,
    "notes": "CPU tests executed. GPU measured and 3GPP conformance are not claimed.",
}
(root / "results" / "supervisor" / "SUPERVISOR_CPU_GATE.json").write_text(json.dumps(summary, indent=2) + "\n")
print(json.dumps(summary, indent=2))
PY

echo "SUPERVISOR_CPU_GATE_OK"
