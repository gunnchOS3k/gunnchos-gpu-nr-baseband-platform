# Reference vector tolerances

| Vector | Metric | Tolerance | Provenance |
|--------|--------|-----------|------------|
| golden_crc24a | bit-exact CRC | exact | self-generated |
| golden_ldpc | bit-exact codeword | exact | self-generated educational (16,8) |
| golden_qpsk | complex abs err | 1e-9 | self-generated |
| optional Sionna/srsRAN/OAI/pyAerial | adapter stubs | DOCUMENTED | see docs/reference/ |

Status: self-generated vectors are authoritative for CI on CPU hosts.
External adapters are optional and may be BLOCKED_HARDWARE / NOT_RUN.
