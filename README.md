# SRA VDB Multithreaded Benchmark

A small C++23 benchmark demonstrating multithreaded processing of NCBI SRA data using the NGS/VDB API and Google Benchmark.

The benchmark divides an SRA read collection into contiguous ranges and processes them concurrently using `std::jthread`.

Each worker:

* Opens its own NGS `ReadCollection`
* Processes an independent read range
* Retrieves DNA bases and quality scores
* Maintains thread-local counters

The benchmark tests 1, 2, 4, 8, and 16 worker threads.

## Requirements

* C++23 compiler
* CMake
* Git
* NCBI NGS/VDB development libraries

On the tested Ubuntu system, the required libraries are:

* `ngs-c++`
* `ncbi-ngs`
* `ncbi-vdb`

Google Benchmark is downloaded automatically by CMake using `FetchContent`.

## Configure

```bash
cmake -S . -B build-bench \
    -DCMAKE_BUILD_TYPE=Release
```

## Build

```bash
cmake --build build-bench -j
```

## Run

```bash
./build-bench/sra_benchmark \
    /path/to/file.sra \
    --benchmark_repetitions=5 \
    --benchmark_report_aggregates_only=true
```

## Example

```bash
./build-bench/sra_benchmark \
    /home/user/SRR29154704/SRR29154704.sra \
    --benchmark_repetitions=5 \
    --benchmark_report_aggregates_only=true
```

## Metrics

The benchmark reports:

* **Time** — wall-clock processing time
* **Mreads/s** — millions of reads processed per second
* **Mbases/s** — millions of DNA bases processed per second

`UseRealTime()` is used because the actual processing occurs in worker threads rather than the Google Benchmark control thread.

## Concurrency

The total read collection is divided into approximately equal contiguous ranges.

Each worker has its own NGS/VDB objects and writes to its own result structure, avoiding synchronization in the read-processing hot path.

`std::jthread` provides RAII-based thread lifetime management and automatically joins worker threads.

