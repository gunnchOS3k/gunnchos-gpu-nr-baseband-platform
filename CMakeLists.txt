cmake_minimum_required(VERSION 3.20)
project(gunnchos_gpu_nr_baseband_platform
  VERSION 0.1.0
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(NR_BB_ENABLE_CUDA "Enable CUDA language and GPU targets" OFF)
option(NR_BB_WARNINGS_AS_ERRORS "Treat warnings as errors" ON)
option(NR_BB_ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(NR_BB_ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)

if(NR_BB_ENABLE_CUDA)
  include(CheckLanguage)
  check_language(CUDA)
  if(CMAKE_CUDA_COMPILER)
    enable_language(CUDA)
    set(NR_BB_HAS_CUDA TRUE)
  else()
    message(WARNING "CUDA requested but no compiler found; building CPU-only")
    set(NR_BB_HAS_CUDA FALSE)
  endif()
else()
  set(NR_BB_HAS_CUDA FALSE)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(NR_BB_SOURCES
  src/crc.cpp
  src/segmentation.cpp
  src/ldpc.cpp
  src/rate_matching.cpp
  src/scrambling.cpp
  src/modulation.cpp
  src/layer_mapping.cpp
  src/dmrs.cpp
  src/ofdm.cpp
  src/channel.cpp
  src/channel_estimation.cpp
  src/equalizer.cpp
  src/llr.cpp
  src/harq.cpp
  src/metrics.cpp
  src/mimo.cpp
  src/scheduler.cpp
  src/fapi.cpp
  src/fronthaul.cpp
)

add_library(nr_bb ${NR_BB_SOURCES})
add_library(nr_bb::nr_bb ALIAS nr_bb)
target_include_directories(nr_bb PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

if(MSVC)
  target_compile_options(nr_bb PRIVATE /W4)
else()
  target_compile_options(nr_bb PRIVATE -Wall -Wextra -Wpedantic)
  if(NR_BB_WARNINGS_AS_ERRORS)
    target_compile_options(nr_bb PRIVATE -Werror)
  endif()
endif()

if(NR_BB_ENABLE_ASAN)
  target_compile_options(nr_bb PUBLIC -fsanitize=address -fno-omit-frame-pointer)
  target_link_options(nr_bb PUBLIC -fsanitize=address)
endif()
if(NR_BB_ENABLE_UBSAN)
  target_compile_options(nr_bb PUBLIC -fsanitize=undefined -fno-omit-frame-pointer)
  target_link_options(nr_bb PUBLIC -fsanitize=undefined)
endif()

if(NR_BB_HAS_CUDA)
  add_library(nr_bb_cuda cuda/scheduler_candidate.cu cuda/benchmark_kernels.cu)
  target_include_directories(nr_bb_cuda PUBLIC include)
  set_target_properties(nr_bb_cuda PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
  target_link_libraries(nr_bb_cuda PUBLIC nr_bb)
endif()

include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.5.4
)
FetchContent_MakeAvailable(Catch2)

enable_testing()

set(NR_BB_TESTS
  tests/test_crc.cpp
  tests/test_segmentation.cpp
  tests/test_ldpc.cpp
  tests/test_rate_matching.cpp
  tests/test_scrambling.cpp
  tests/test_modulation.cpp
  tests/test_layer_mapping.cpp
  tests/test_dmrs.cpp
  tests/test_ofdm.cpp
  tests/test_channel.cpp
  tests/test_channel_estimation.cpp
  tests/test_equalizer.cpp
  tests/test_llr.cpp
  tests/test_harq.cpp
  tests/test_metrics.cpp
  tests/test_mimo.cpp
  tests/test_scheduler.cpp
  tests/test_fapi.cpp
  tests/test_fronthaul.cpp
  tests/test_vectors.cpp
)

add_executable(nr_bb_tests ${NR_BB_TESTS})
target_link_libraries(nr_bb_tests PRIVATE nr_bb Catch2::Catch2WithMain)
target_compile_definitions(nr_bb_tests PRIVATE
  NR_BB_VECTORS_DIR="${CMAKE_SOURCE_DIR}/vectors")

include(Catch)
catch_discover_tests(nr_bb_tests)

add_executable(nr_bb_bench benchmarks/bench_harness.cpp)
target_link_libraries(nr_bb_bench PRIVATE nr_bb)
target_compile_definitions(nr_bb_bench PRIVATE
  NR_BB_RESULTS_DIR="${CMAKE_SOURCE_DIR}/results/benchmarks")

add_executable(nr_bb_fuzz_fapi fuzz/fuzz_fapi.cpp)
target_link_libraries(nr_bb_fuzz_fapi PRIVATE nr_bb)

install(TARGETS nr_bb ARCHIVE DESTINATION lib LIBRARY DESTINATION lib)
install(DIRECTORY include/nr_bb DESTINATION include)

message(STATUS "nr_bb CUDA enabled: ${NR_BB_HAS_CUDA}")
