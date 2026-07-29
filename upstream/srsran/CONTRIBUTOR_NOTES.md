# Contributor notes (srsRAN-oriented)

Status: **DOCUMENTED_IMPLEMENTATION** only — do not present as merged upstream.

Proposed contribution: FAPI message sequencing validator inspired by this repo’s
`nr_bb::FapiSession` bounded state machine and JSONL fixtures.

Suggested integration point: test utility under srsRAN Project test suite for
CONFIG → START → UL/DL_TTI → TX_DATA → CRC indication ordering.
