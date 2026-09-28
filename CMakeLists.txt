cmake_minimum_required(VERSION 3.20)

project(sra_benchmark LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------------------------------------------------------
# Google Benchmark
# ---------------------------------------------------------------------------

include(FetchContent)

set(BENCHMARK_ENABLE_TESTING OFF)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF)
set(BENCHMARK_ENABLE_INSTALL OFF)

FetchContent_Declare(
    benchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.9.4
)

FetchContent_MakeAvailable(benchmark)

# ---------------------------------------------------------------------------
# SRA benchmark
# ---------------------------------------------------------------------------

add_executable(sra_benchmark
    sra_benchmark.cpp
)

# NGS.hpp from the Ubuntu NCBI VDB development package lives here.
target_include_directories(sra_benchmark PRIVATE
    /usr/include/ncbi-vdb
)

find_package(Threads REQUIRED)

target_link_libraries(sra_benchmark PRIVATE
    benchmark::benchmark

    # NCBI NGS / VDB libraries.
    ngs-c++
    ncbi-ngs
    ncbi-vdb

    Threads::Threads
)