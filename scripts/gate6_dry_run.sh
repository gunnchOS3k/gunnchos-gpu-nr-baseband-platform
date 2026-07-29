#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
mkdir -p results/gate6_dry_run
TS="$(date -u +%Y-%m-%dT%H:%M:%SZ)"
HAS_GPU=0
if command -v nvidia-smi >/dev/null 2>&1; then HAS_GPU=1; fi

write_blocked() {
  local name="$1" notes="$2"
  cat >"results/gate6_dry_run/${name}.json" <<JSON
{
  "artifact": "$name",
  "gate": "gate6-dry-run",
  "status": "BLOCKED_HARDWARE",
  "fail_closed": true,
  "physical_pass": false,
  "gpu_timings_present": false,
  "nic_ptp_measured": false,
  "sdr_ru_measured": false,
  "notes": "$notes",
  "timestamp": "$TS"
}
JSON
}

# Always FAIL-CLOSED dry-run: no fabricated physical PASS
write_blocked "gpu_nsight_dry_run" "No Nsight / NVIDIA GPU numeric evidence on this host; dry-run only"
write_blocked "nic_ptp_dry_run" "NIC/PTP lab path not instrumented; PENDING_LAB / dry-run only"
write_blocked "fh_loss_dry_run" "Fronthaul impairment dry-run registry only; not physical FH pass"
write_blocked "sdr_ru_dry_run" "SDR/RU cabled lab PENDING_LAB"

if [[ "$HAS_GPU" -eq 0 ]]; then
  STATUS="BLOCKED_HARDWARE"
  NOTES="Apple Silicon / no nvidia-smi — Gate 6 physical PASS impossible; dry-run structured pending only"
else
  STATUS="PENDING_LAB"
  NOTES="GPU present but Gate 6 still requires signed lab protocol evidence — dry-run only"
fi

cat >results/gate6_dry_run/gate6_dry_run_summary.json <<JSON
{
  "gate": "gate6-dry-run",
  "status": "$STATUS",
  "fail_closed": true,
  "physical_pass": false,
  "gpu_present": $([[ "$HAS_GPU" -eq 1 ]] && echo true || echo false),
  "artifacts": [
    "gpu_nsight_dry_run.json",
    "nic_ptp_dry_run.json",
    "fh_loss_dry_run.json",
    "sdr_ru_dry_run.json"
  ],
  "notes": "$NOTES",
  "timestamp": "$TS"
}
JSON

echo "gate6-dry-run complete status=$STATUS physical_pass=false"
cat results/gate6_dry_run/gate6_dry_run_summary.json
