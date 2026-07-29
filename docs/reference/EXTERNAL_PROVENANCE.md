# External reference provenance paths

Educational adapters only — **not** embedded third-party code.

| Source | Path / notes | Adapter status |
|--------|--------------|----------------|
| NVIDIA Sionna | Compare LLR / LDPC short-block BERs offline | stub: `python/nr_bb/adapters/sionna_stub.py` |
| srsRAN Project | PHY / FAPI conceptual cross-check | stub + `upstream/srsran/` packet |
| OpenAirInterface (OAI) | MAC/PHY timing / FH concepts | stub + `upstream/oai/` packet |
| pyAerial / Aerial | GPU kernel / pipeline inspiration | DOCUMENTED; GPU runs `BLOCKED_HARDWARE` on Mac |

Self-generated golden vectors under `vectors/` are CI-authoritative.
Tolerances: `vectors/TOLERANCES.md`.
