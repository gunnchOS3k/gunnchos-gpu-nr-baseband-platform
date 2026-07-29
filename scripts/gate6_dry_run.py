#!/usr/bin/env python3
"""Gate 6 dry-run for NVIDIA NR baseband platform — harness only, no GPU/physical PASS."""
from __future__ import annotations

import json
from datetime import datetime, timezone
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def utc_now() -> str:
    return datetime.now(timezone.utc).replace(microsecond=0).isoformat().replace("+00:00", "Z")


def main() -> int:
    report = {
        "ok": True,
        "gate": "6",
        "repository": "gunnchos-gpu-nr-baseband-platform",
        "mode": "dry_run",
        "evidence_label": "SYNTHETIC_EXPERIMENT",
        "physical_pass": False,
        "started": utc_now(),
        "statuses": {
            "GATE6_HARNESS": "GATE6_PARTIAL_HARNESS_PASS",
            "GPU_MEASUREMENT": "GPU_MEASUREMENT_PENDING",
            "NIC_PTP": "NIC_PTP_PENDING",
            "SDR_LAB": "SDR_LAB_PENDING",
            "PHYSICAL_EVIDENCE": "PHYSICAL_EVIDENCE_PENDING",
        },
        "hardware": {
            "nvidia_gpu_present": False,
            "evidence_label": "BLOCKED_HARDWARE",
            "note": "No nvidia-smi / CUDA device on author host; dry-run must not emit GPU timings",
        },
        "claim": "GATE6 dry-run only — no Aerial equivalence, no GPU PASS, no NIC/PTP PASS",
        "finished": utc_now(),
    }
    for path in (
        ROOT / "physical_evidence" / "GATE6_DRY_RUN_REPORT.json",
        ROOT / "physical_evidence" / "gate6_dry_run_report.json",
        ROOT / "results" / "gate6" / "gate6_dry_run_report.json",
    ):
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"ok": True, "report": str(ROOT / "physical_evidence" / "GATE6_DRY_RUN_REPORT.json")}, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
