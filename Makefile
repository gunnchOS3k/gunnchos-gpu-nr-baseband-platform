.PHONY: bootstrap test smoke reproduce paper artifact gate4-cpu gate4-gpu profile-gpu benchmark-gpu format

bootstrap:
	cmake --preset cpu
	cmake --build --preset cpu

test: bootstrap
	ctest --preset cpu --output-on-failure

smoke: test
	cmake --build --preset cpu --target nr_bb_bench
	./build/cpu/nr_bb_bench
	./build/cpu/nr_bb_fuzz_fapi

reproduce:
	./scripts/reproduce.sh

paper:
	@test -f paper/CPU_GPU_NIC_NR_BASEBAND_BENCHMARK.md && echo "Paper present" || (echo "missing paper"; exit 1)

artifact: smoke
	./scripts/make_artifact.sh

gate4-cpu:
	./scripts/gate4_cpu.sh

gate4-gpu:
	./scripts/gate4_gpu.sh

profile-gpu:
	./scripts/profile_gpu.sh

benchmark-gpu: gate4-gpu

format:
	@command -v clang-format >/dev/null && find include src tests benchmarks fuzz -name '*.hpp' -o -name '*.cpp' | xargs clang-format -i || echo "clang-format not installed"

.PHONY: gate6-dry-run
gate6-dry-run:
	python3 scripts/gate6_dry_run.py
