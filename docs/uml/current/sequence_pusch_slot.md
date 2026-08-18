# Sequence — slot / PUSCH (CPU standards-path)

FAPI-like slot indication driving a PUSCH-oriented UL chain. This is the **implemented software order**, not a claim of a certified gNB.

```mermaid
sequenceDiagram
  participant MAC as FapiSession
  participant PHY as nr_bb standards-path
  participant VEC as Golden vectors
  MAC->>MAC: CONFIG_REQ / START_REQ
  MAC->>PHY: SLOT_IND (sfn, slot)
  MAC->>PHY: UL_TTI_REQ (PUSCH grant)
  PHY->>PHY: CRC attach
  PHY->>PHY: Segmentation
  PHY->>PHY: LDPC encode (BG1/BG2)
  PHY->>PHY: Rate match, scramble, modulate
  PHY->>PHY: Layer map, DMRS, IFFT/OFDM
  Note over PHY: Channel model is CPU synthetic here
  PHY->>PHY: FFT, channel estimate, equalize
  PHY->>PHY: LLR, descramble, rate dematch
  PHY->>PHY: LDPC decode, CRC check
  PHY->>MAC: RX_DATA_IND / CRC_IND
  PHY->>VEC: Compare CRC / QPSK / BG2 (class-labeled)
  MAC->>MAC: STOP_REQ
```

Educational CompactQcLdpc / (16,8) is a **parallel** test path, not this sequence.
