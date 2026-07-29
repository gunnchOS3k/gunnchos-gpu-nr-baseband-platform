# Draft issue: FAPI sequencing validator (research port)

## Summary
Research-bounded FAPI-like state machine often accepts out-of-order TX before
CONFIG/START. Propose a small JSONL-driven sequencing validator with negative
fixtures for education / integration tests.

## Acceptance
- Happy path CONFIG→START→TX
- TX before START → ERROR
- Version field checked
- No performance regression

**Upstream status:** DOCUMENTED_IMPLEMENTATION (not filed yet).
