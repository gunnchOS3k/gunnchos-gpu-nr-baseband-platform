# SDR / RU cabled-test protocol

1. Use cabled IF/IQ or digital loopback only unless RF is authorized.
2. Replay IQ through `nr_bb::FronthaulEmulator`; capture PCAP dumps.
3. Inject loss/reorder/delay/corruption; record fault-injection log.
4. See `AUTHORIZED_RF_WARNING.md`.
