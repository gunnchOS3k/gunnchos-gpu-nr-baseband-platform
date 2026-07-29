"""MATLAB / 5G Toolbox reference adapter."""
from __future__ import annotations

import json
import shutil
from pathlib import Path

ADAPTER = "matlab"
DEPENDENCY = "MATLAB + 5G Toolbox"


def run_comparison(out_path: Path) -> dict:
    report = {
        "adapter": ADAPTER,
        "dependency": DEPENDENCY,
        "vectors": ["nrLDPCEncode", "nrRateMatchLDPC"],
        "metrics": {},
        "notes": "",
    }
    if not shutil.which("matlab"):
        report["status"] = "REFERENCE_DEPENDENCY_UNAVAILABLE"
        report["notes"] = "MATLAB not on PATH; comparison pending."
    else:
        report["status"] = "REFERENCE_DEPENDENCY_UNAVAILABLE"
        report["notes"] = "MATLAB present but 5G Toolbox golden script not executed here."
    out_path.write_text(json.dumps(report, indent=2) + "\n")
    return report


if __name__ == "__main__":
    out = Path(__file__).resolve().parents[3] / "results/phy_independent/matlab.json"
    out.parent.mkdir(parents=True, exist_ok=True)
    print(json.dumps(run_comparison(out), indent=2))
