# Sionna 5G NR LDPC base-graph tables (vendored)

| File | Role |
|------|------|
| `5G_bg1.csv` / `5G_bg2.csv` | Circular-shift coefficients for BG1/BG2, all `i_LS` ∈ {0…7} |
| `NOTICE` | Apache-2.0 license text from upstream Sionna |

**Upstream:** [NVlabs/sionna](https://github.com/NVlabs/sionna) `src/sionna/phy/fec/ldpc/codes/`  
**License:** Apache-2.0  
**Spec correspondence:** 3GPP TS 38.212 Tables 5.3.2-2 / 5.3.2-3  

These CSVs are **not** NVIDIA Aerial / cuBB proprietary code. They are used only as a
permissively licensed transcription of public 3GPP base-graph coefficients.

Regenerate C++ tables:

```bash
python3 scripts/generate_ldpc_bg_tables.py
```
