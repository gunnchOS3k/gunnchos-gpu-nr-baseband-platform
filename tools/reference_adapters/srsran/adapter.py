"""srsRAN reference adapter — FAPI / PHY vector compare when srsRAN tools exist."""
from __future__ import annotations

import json
import shutil
from pathlib import Path

ADAPTER = "srsran"
DEPENDENCY = "srsRAN Project CLI / libraries"


def run_comparison(out_path: Path) -> dict:
    report = {
        "adapter": ADAPTER,
        "dependency": DEPENDENCY,
        "vectors": ["fapi_sequencing", "scramble_gold"],
        "metrics": {},
        "notes": "",
    }
    found = any(shutil.which(x) for x in ("srsran", "gnb", "srsue"))
    if not found:
        report["status"] = "REFERENCE_DEPENDENCY_UNAVAILABLE"
        report["notes"] = "srsRAN binaries not on PATH; comparison pending."
    else:
        report["status"] = "REFERENCE_DEPENDENCY_UNAVAILABLE"
        report["notes"] = "srsRAN detected but shared golden harness not executed here."
    out_path.write_text(json.dumps(report, indent=2) + "\n")
    return report


if __name__ == "__main__":
    out = Path(__file__).resolve().parents[3] / "results/phy_independent/srsran.json"
    out.parent.mkdir(parents=True, exist_ok=True)
    print(json.dumps(run_comparison(out), indent=2))
