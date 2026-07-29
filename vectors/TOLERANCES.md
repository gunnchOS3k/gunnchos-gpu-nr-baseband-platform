# Reference vector tolerances

| Vector | Metric | Tolerance | Provenance |
|--------|--------|-----------|------------|
| golden_crc24a | bit-exact CRC | exact | self-generated CRC24A |
| golden_ldpc_edu | bit-exact (16,8) | exact | EDUCATIONAL_ONLY |
| golden_nr_ldpc_bg2 | syndrome OK + decode | exact under noiseless LLR | self-generated BG2-compact zc=2 |
| golden_qpsk | complex abs err | 1e-9 | Gray QPSK self-generated |

Educational vectors must not be sole Gate 4 acceptance criteria.
External adapters: DOCUMENTED stubs — see `docs/reference/EXTERNAL_PROVENANCE.md`.
