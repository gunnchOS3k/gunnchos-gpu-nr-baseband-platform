# Independent PHY reference adapters

Adapters under `tools/reference_adapters/{sionna,srsran,oai,matlab}/` compare this
repo's standards-path PHY against external references **when dependencies exist**.

## Provenance schema (JSON)

```json
{
  "adapter": "sionna|srsran|oai|matlab",
  "status": "REFERENCE_ADAPTER_IMPLEMENTED|REFERENCE_DEPENDENCY_UNAVAILABLE|COMPARISON_PASS|COMPARISON_FAIL",
  "dependency": "...",
  "vectors": [],
  "metrics": {},
  "notes": "..."
}
```

## PASS rule

`make validate-phy-independent` requires **≥2** adapters reporting `COMPARISON_PASS`.
On Apple M2 without Sionna/srsRAN/OAI/MATLAB installed, honest result is
`REFERENCE_DEPENDENCY_UNAVAILABLE` → overall **pending** (not a fabricated PASS).
