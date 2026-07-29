"""OpenAirInterface reference adapter."""
from __future__ import annotations

import json
import shutil
from pathlib import Path

ADAPTER = "oai"
DEPENDENCY = "OpenAirInterface nr_ulsim / softmodem"


def run_comparison(out_path: Path) -> dict:
    report = {
        "adapter": ADAPTER,
        "dependency": DEPENDENCY,
        "vectors": ["pusch_bler_curve", "timing_json"],
        "metrics": {},
        "notes": "",
    }
    found = any(shutil.which(x) for x in ("nr_ulsim", "nr-softmodem", "oai"))
    if not found:
        report["status"] = "REFERENCE_DEPENDENCY_UNAVAILABLE"
        report["notes"] = "OAI tools not on PATH; comparison pending."
    else:
        report["status"] = "REFERENCE_DEPENDENCY_UNAVAILABLE"
        report["notes"] = "OAI detected but shared golden harness not executed here."
    out_path.write_text(json.dumps(report, indent=2) + "\n")
    return report


if __name__ == "__main__":
    out = Path(__file__).resolve().parents[3] / "results/phy_independent/oai.json"
    out.parent.mkdir(parents=True, exist_ok=True)
    print(json.dumps(run_comparison(out), indent=2))
