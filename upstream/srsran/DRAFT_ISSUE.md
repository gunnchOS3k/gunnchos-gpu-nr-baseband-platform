# Draft issue: FAPI sequencing validator helper

## Summary
Add an optional test helper that validates FAPI-like message ordering for
CONFIG/START/TTI/TX_DATA/CRC paths using fixtures.

## Motivation
Catch state-machine regressions in integration tests.

## Out of scope
Not a claim of SCF FAPI certification.
