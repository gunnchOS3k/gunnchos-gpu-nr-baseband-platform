# Gate 6 — GPU lab protocol

1. Record `scripts/nvidia_manifest.sh` and driver versions.
2. Build with `cmake --preset gpu`.
3. Run Nsight Systems (`nsys`) and Compute (`ncu`) via `scripts/profile_gpu.sh` extensions.
4. Capture thermal/power JSON per `THERMAL_POWER_SCHEMA.md`.
5. If no GPU: write `BLOCKED_HARDWARE` and stop — do not invent numbers.
