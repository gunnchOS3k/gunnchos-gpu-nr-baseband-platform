# Deployment — CPU vs GPU

```mermaid
flowchart TB
  subgraph local [Author / CI CPU host]
    M2[Apple Silicon or ubuntu-latest]
    CMAKE[cmake --preset cpu]
    TEST[ctest --preset cpu]
    JSON1[results/blocked_gpu/BLOCKED_GPU.json]
    M2 --> CMAKE --> TEST --> JSON1
  end
  subgraph lab [NVIDIA lab — not this Mac]
    RUN[self-hosted labels: self-hosted, nvidia-gpu]
    NVCC[nvcc + nvidia-smi]
    NS[Nsight / gate4-gpu JSON]
    RUN --> NVCC --> NS
  end
  subgraph gh [GitHub]
    CI[.github/workflows/ci.yml cpu job]
    GPUWF[self-hosted-nvidia-gpu.yml stub]
    CI --> TEST
    GPUWF --> JSON1
  end
```

The `gpu` CMake preset and `cuda/` sources are **candidates** on CPU-only machines. GitHub `ubuntu-latest` GPU jobs emit `BLOCKED_GPU_RUNNER` unless a labeled runner is attached.
