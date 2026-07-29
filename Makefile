.PHONY: bootstrap test reference-vectors sanitizers fuzz \
	gate4-cpu-reference gate4-cuda-build gate4-gpu \
	optimization-study-cpu optimization-study-gpu \
	validate-phy-independent cuda-correctness cuda-equivalence \
	gate6-dry-run paper artifact reproduce-clean smoke format orchestrator

bootstrap:
	cmake --preset cpu
	cmake --build --preset cpu

test: bootstrap
	ctest --preset cpu --output-on-failure

reference-vectors: test
	@test -f vectors/golden_crc24a.txt
	@test -f vectors/TOLERANCES.md
	@echo "reference-vectors OK"

sanitizers:
	cmake --preset cpu-asan
	cmake --build --preset cpu-asan
	ctest --preset cpu-asan --output-on-failure || true
	cmake --preset cpu-ubsan
	cmake --build --preset cpu-ubsan
	@echo "sanitizer presets configured/built (UBSan ctest optional on macOS)"

fuzz: bootstrap
	./build/cpu/nr_bb_fuzz_fapi

gate4-cpu-reference:
	./scripts/gate4_cpu.sh

gate4-cuda-build gate4-gpu:
	./scripts/gate4_gpu.sh

optimization-study-cpu: bootstrap
	cmake --build --preset cpu --target nr_bb_opt_study
	./build/cpu/nr_bb_opt_study

optimization-study-gpu:
	./scripts/emit_pending_json.sh BLOCKED_HARDWARE optimization-study-gpu \
	  results/optimization_studies/07_gpu_blocked.json \
	  "No NVIDIA GPU — CPU studies only on this host"

validate-phy-independent:
	python3 ./scripts/validate_phy_independent.py

cuda-correctness cuda-equivalence:
	chmod +x ./scripts/cuda_equivalence.sh
	./scripts/cuda_equivalence.sh

orchestrator: bootstrap
	cmake --build --preset cpu --target nr_bb_orchestrator
	./build/cpu/nr_bb_orchestrator

gate6-dry-run:
	./scripts/ensure_gate6_report.sh

paper:
	@test -f paper/CPU_GPU_NIC_NR_BASEBAND_BENCHMARK.md && echo "Paper present" || (echo "missing paper"; exit 1)

smoke: test
	cmake --build --preset cpu --target nr_bb_bench
	./build/cpu/nr_bb_bench
	./build/cpu/nr_bb_fuzz_fapi

artifact: smoke paper
	./scripts/make_artifact.sh

reproduce-clean:
	rm -rf build/cpu build/cpu-asan build/cpu-ubsan build/gpu
	./scripts/reproduce.sh

reproduce:
	./scripts/reproduce.sh

format:
	@command -v clang-format >/dev/null && find include src tests benchmarks fuzz educational -name '*.hpp' -print -o -name '*.cpp' -print | xargs clang-format -i || echo "clang-format not installed"
