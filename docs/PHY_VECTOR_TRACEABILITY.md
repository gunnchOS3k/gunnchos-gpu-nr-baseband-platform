# PHY vector traceability

Golden files are the CPU-validation contract. Educational vectors never stand in for NR acceptance.

| File | Class | Metric / tolerance | Consumed by | Provenance |
|---|---|---|---|---|
| `vectors/golden_crc24a.txt` | CPU validation / standards-path | bit-exact CRC24A | `tests/test_vectors.cpp`, `tests/test_crc.cpp` | self-generated CRC24A |
| `vectors/golden_qpsk.txt` | CPU validation / standards-path | complex abs err ≤ 1e-9 | `tests/test_modulation.cpp` | Gray QPSK self-generated |
| `vectors/golden_ldpc.txt` | **educational substitute** | bit-exact (16,8) | `tests/test_vectors.cpp`, educational tests | `EDUCATIONAL_ONLY` — not Gate 4 Track C |
| `vectors/nr_ldpc/golden_bg2_zc2.txt` | standards-path scaffold | syndrome OK under noiseless LLR | `tests/test_ldpc.cpp` | BG2 compact / TS 38.212 lifting mechanics; self-generated CW |
| `vectors/TOLERANCES.md` | documentation | table of tolerances | humans + CI readers | repo policy |
| `third_party/sionna_ldpc_codes/` | standards-path tables | BG CSV → `ldpc_bg_tables.hpp` | LDPC encode/decode | Apache-2.0 Sionna; see `docs/reference/LDPC_SOURCE_AND_LICENSE_AUDIT.md` |

Independent adapters (`tools/reference_adapters/{sionna,srsran,oai,matlab}`) write `results/phy_independent/*.json`. Overall `COMPARISON_PASS` requires ≥2 adapters; otherwise `REFERENCE_DEPENDENCY_UNAVAILABLE` / `PENDING` is the honest CPU result.

SHA-256 of vector files is emitted by `scripts/supervisor_cpu_gate.sh` into `results/supervisor/phy_vector_hashes.json` so a reproducer can detect drift.

## Non-claims

- Matching educational (16,8) LDPC is **not** NR LDPC acceptance.
- Matching CPU CRC/QPSK/BG2-small is **not** 3GPP conformance.
- Adapter `PENDING` is not a hidden FAIL; it means the external toolchain is not on this host.
