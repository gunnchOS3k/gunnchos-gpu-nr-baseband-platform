# Upstream contribution packet — srsRAN (bounded)

**Status:** `DOCUMENTED_IMPLEMENTATION` until merge upstream.

## Scope (bounded)

Add a **FAPI sequencing validator** (JSONL fixtures + negative tests for TX before
START) ported from this research SM. No cuBB / proprietary NVIDIA code.

## Files in this packet

| File | Role |
|------|------|
| `DRAFT_ISSUE.md` | Proposed issue text |
| `DRAFT_PR.md` | Proposed PR body |
| `fapi_sequencing_validator.patch` | Illustrative patch against a research shim |
| `CONTRIBUTOR_NOTES.md` | License / provenance |

## Provenance

- Research implementation under MIT in `gunnchos-gpu-nr-baseband-platform`.
- Target project licenses (srsRAN Project) must be respected on submission.
- Do not copy 3GPP specification text verbatim beyond necessary identifiers.

## Merge checklist

- [ ] Open issue on srsRAN Project
- [ ] Rebase patch on current `main`
- [ ] CI green on their runners
- [ ] Update this file status from DOCUMENTED_IMPLEMENTATION → MERGED with URL
