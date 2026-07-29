#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
"$ROOT/scripts/gate6_dry_run.sh" || true
python3 "$ROOT/scripts/gate6_dry_run.py"
