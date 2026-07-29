# GPU lab image — requires NVIDIA Container Toolkit on the host.
# On Apple Silicon this Dockerfile is documentation-only (BLOCKED_HARDWARE).
FROM nvidia/cuda:12.3.0-devel-ubuntu22.04
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
    cmake ninja-build g++ git python3 && rm -rf /var/lib/apt/lists/*
WORKDIR /src
COPY . .
RUN cmake --preset gpu && cmake --build --preset gpu
CMD ["ctest", "--preset", "gpu", "--output-on-failure"]
