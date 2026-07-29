# Reference vector tolerances

| Vector | Metric | Tolerance | Provenance |
|--------|--------|-----------|------------|
| golden_crc24a | bit-exact CRC | exact | self-generated CRC24A |
| golden_ldpc_edu | bit-exact (16,8) | exact | EDUCATIONAL_ONLY |
| golden_nr_ldpc_bg2 | syndrome OK + decode | exact under noiseless LLR | real NR BG2 tables (Sionna CSV / TS 38.212), self-generated CW |
| golden_qpsk | complex abs err | 1e-9 | Gray QPSK self-generated |

Educational CompactQcLdpc / (16,8) vectors must not be sole Gate 4 / Track C acceptance criteria.
External adapters: see `tools/reference_adapters/` and `docs/reference/EXTERNAL_PROVENANCE.md`.
