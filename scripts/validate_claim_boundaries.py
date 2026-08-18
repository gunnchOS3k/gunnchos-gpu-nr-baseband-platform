#!/usr/bin/env python3
"""Ensure claim-boundary docs and labels stay separated."""
from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
REQUIRED_CLASSES = (
    "educational substitute",
    "standards-path",
    "CPU validation",
    "CUDA candidate",
    "GPU measured",
    "3GPP conformance",
)


def main() -> int:
    text = (ROOT / "docs" / "CLAIM_BOUNDARIES.md").read_text(encoding="utf-8").lower()
    missing = [c for c in REQUIRED_CLASSES if c.lower() not in text]
    uml = ROOT / "docs" / "uml" / "current"
    needed_uml = [
        "component.md",
        "class_phy.md",
        "sequence_pusch_slot.md",
        "timing.md",
        "deployment.md",
        "state_validation_gate.md",
    ]
    uml_missing = [n for n in needed_uml if not (uml / n).is_file()]
    vis = ROOT / "docs" / "packets" / "REPOSITORY_VISIBILITY_PACKET.md"
    repro = ROOT / "REPRODUCIBILITY.md"
    errors = []
    if missing:
        errors.append(f"CLAIM_BOUNDARIES missing classes: {missing}")
    if uml_missing:
        errors.append(f"UML missing: {uml_missing}")
    if not vis.is_file():
        errors.append("missing REPOSITORY_VISIBILITY_PACKET.md")
    if not repro.is_file():
        errors.append("missing REPRODUCIBILITY.md")
    if "never change github visibility" not in vis.read_text(encoding="utf-8").lower():
        errors.append("visibility packet must forbid visibility changes")
    if errors:
        print("FAIL")
        for e in errors:
            print(" -", e)
        return 1
    print("CLAIM_BOUNDARIES_OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
