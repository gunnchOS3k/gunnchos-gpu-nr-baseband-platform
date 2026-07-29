# Connection diagrams (markdown)

## CPU development loopback

```
[Host CPU nr_bb] --FAPI-like-- [Scheduler]
       |
       +-- IQ vectors -- [Fronthaul emulator] -- PCAP dump
```

## GPU / NIC lab (pending)

```
[DU host + GPU] -- eCPRI/FH -- [NIC] -- cable -- [RU/SDR]
                      |
                   PTP / sync
```

Fill physical ports only during lab bring-up; until then mark `PENDING_LAB`.
