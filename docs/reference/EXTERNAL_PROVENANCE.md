# External reference provenance paths

| Source | Path / notes | Adapter status |
|--------|--------------|----------------|
| NVIDIA Sionna | Compare LLR / LDPC BERs offline | stub: `python/nr_bb/adapters/` |
| srsRAN Project | FAPI sequencing contribution packet | `upstream/srsran/` DOCUMENTED_IMPLEMENTATION |
| OpenAirInterface | timing JSON exporter packet | `upstream/oai/` DOCUMENTED_IMPLEMENTATION |
| pyAerial / Aerial | GPU pipeline inspiration only | DOCUMENTED; GPU `BLOCKED_HARDWARE` on Mac |

Self-generated vectors under `vectors/` are CI-authoritative for the standards path.
Educational (16,8) LDPC vectors are labeled EDUCATIONAL_ONLY.
NO proprietary NVIDIA source is vendored.
