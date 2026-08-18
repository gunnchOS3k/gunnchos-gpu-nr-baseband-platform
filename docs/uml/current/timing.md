# Timing — numerology deadlines vs evidence class

CPU `deadline.hpp` derives slot duration from μ (`15·2^μ` kHz). Samples fed into `evaluate_deadlines` are **CPU synthetic** unless a GPU-measured JSON exists.

```mermaid
flowchart LR
  subgraph slot [Slot budget]
    MU0["μ=0 → 1000 μs"]
    MU1["μ=1 → 500 μs"]
    MU2["μ=2 → 250 μs"]
  end
  subgraph cpu [CPU validation]
    BENCH[nr_bb_bench / orchestrator]
    REP[DeadlineReport timing_source=cpu_synthetic]
  end
  subgraph gpu [GPU measured]
    NS[Nsight JSON]
  end
  MU0 --> BENCH
  MU1 --> BENCH
  MU2 --> BENCH
  BENCH --> REP
  NS -.->|"only if NVIDIA host"| REP
```

```mermaid
sequenceDiagram
  participant Slot as Slot timer
  participant CPU as CPU pipeline
  participant GPU as CUDA candidate
  Slot->>CPU: t=0 start
  CPU->>CPU: PUSCH chain (Catch2 / bench)
  CPU-->>Slot: median_us / p99_us labeled cpu_synthetic
  Note over GPU: Absent nvcc → BLOCKED_GPU, no timestamps
```

Never copy CPU microseconds into a GPU results file.
