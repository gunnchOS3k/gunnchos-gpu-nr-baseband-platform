#!/usr/bin/env bash
# Emit FAIL-CLOSED / BLOCKED_HARDWARE structured JSON (never invent GPU numbers).
set -euo pipefail
status="${1:?status}"
gate="${2:?gate}"
out="${3:?outfile}"
notes="${4:-}"
mkdir -p "$(dirname "$out")"
ts="$(date -u +%Y-%m-%dT%H:%M:%SZ)"
cat >"$out" <<JSON
{
  "gate": "$gate",
  "status": "$status",
  "fail_closed": true,
  "physical_pass": false,
  "gpu_timings_present": false,
  "notes": "$notes",
  "host": "$(uname -s)-$(uname -m)",
  "timestamp": "$ts"
}
JSON
echo "Wrote $out ($status)"
