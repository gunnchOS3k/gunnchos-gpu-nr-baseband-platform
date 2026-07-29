#!/usr/bin/env python3
"""Run all independent PHY adapters and enforce ≥2 COMPARISON_PASS for overall PASS."""
from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "results" / "phy_independent"
ADAPTER_ROOT = ROOT / "tools" / "reference_adapters"


def _load(name: str):
    path = ADAPTER_ROOT / name / "adapter.py"
    spec = importlib.util.spec_from_file_location(f"adapter_{name}", path)
    mod = importlib.util.module_from_spec(spec)
    assert spec.loader is not None
    spec.loader.exec_module(mod)
    return mod


def main() -> int:
    OUT.mkdir(parents=True, exist_ok=True)
    reports = []
    for name in ("sionna", "srsran", "oai", "matlab"):
        mod = _load(name)
        reports.append(mod.run_comparison(OUT / f"{name}.json"))

    passes = sum(1 for r in reports if r.get("status") == "COMPARISON_PASS")
    unavailable = sum(1 for r in reports if r.get("status") == "REFERENCE_DEPENDENCY_UNAVAILABLE")
    fails = sum(1 for r in reports if r.get("status") == "COMPARISON_FAIL")

    if passes >= 2 and fails == 0:
        overall = "PASS"
        notes = f"{passes} adapters COMPARISON_PASS"
    elif unavailable == len(reports) or (passes < 2 and fails == 0):
        overall = "PENDING"
        notes = (
            f"Honest pending: comparison_pass={passes}/2 required; "
            f"unavailable={unavailable} (typical on Apple M2 without deps)"
        )
    else:
        overall = "FAIL"
        notes = f"passes={passes} fails={fails} unavailable={unavailable}"

    summary = {
        "status": overall,
        "comparison_pass_count": passes,
        "required_pass_count": 2,
        "adapters": reports,
        "notes": notes,
    }
    (OUT / "summary.json").write_text(json.dumps(summary, indent=2) + "\n")
    print(json.dumps(summary, indent=2))
    return 0 if overall in ("PASS", "PENDING") else 1


if __name__ == "__main__":
    raise SystemExit(main())
