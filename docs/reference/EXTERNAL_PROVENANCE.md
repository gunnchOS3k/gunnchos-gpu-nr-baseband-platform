# External reference provenance paths

| Source | Path / notes | Adapter status |
|--------|--------------|----------------|
| NVIDIA Sionna | BG CSV tables (Apache-2.0) + optional BER compare | `tools/reference_adapters/sionna/` |
| srsRAN Project | FAPI / PHY compare when installed | `tools/reference_adapters/srsran/` |
| OpenAirInterface | timing / PUSCH compare when installed | `tools/reference_adapters/oai/` |
| MATLAB 5G Toolbox | nrLDPCEncode compare when installed | `tools/reference_adapters/matlab/` |
| pyAerial / Aerial | GPU pipeline inspiration only | DOCUMENTED; GPU `BLOCKED_HARDWARE` on Mac |

`make validate-phy-independent` requires ≥2 `COMPARISON_PASS` for PASS; otherwise
honest `REFERENCE_DEPENDENCY_UNAVAILABLE` / PENDING.

Self-generated vectors under `vectors/` are CI-authoritative for the standards path.
CompactQcLdpc / (16,8) vectors are labeled EDUCATIONAL_ONLY.
NO proprietary NVIDIA Aerial / cuBB source is vendored.
