# SRA VDB Multithreaded Benchmark

A small C++23 benchmark demonstrating multithreaded processing of NCBI SRA data using the NGS/VDB API and Google Benchmark.

The benchmark divides an SRA read collection into contiguous ranges and processes them concurrently using `std::jthread`.

Each worker:

- Opens its own NGS `ReadCollection`
- Processes an independent read range
- Retrieves DNA bases and quality scores
- Maintains thread-local counters

The benchmark tests 1, 2, 4, 8, and 16 worker threads.

## Requirements

- C++23 compiler
- CMake
- Git
- NCBI NGS/VDB development libraries

On the tested Ubuntu system, the required libraries are:

- `ngs-c++`
- `ncbi-ngs`
- `ncbi-vdb`

Google Benchmark is downloaded automatically by CMake using `FetchContent`.

## Build

```bash
cmake -S . -B build-bench -DCMAKE_BUILD_TYPE=Release
cmake --build build-bench -j
