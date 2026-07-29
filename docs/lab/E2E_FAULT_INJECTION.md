# E2E fault-injection protocol

Faults (fronthaul emulator): packet loss, reorder, delay, corruption.

Procedure:

1. Baseline loopback PCAP + CRC/BLER metrics.
2. Enable one impairment at a time; sweep probability.
3. Record JSON result with seed, impairment, metrics.
4. Dry-run fixtures live in `results/gate6_dry_run/`.
