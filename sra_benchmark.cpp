#include <NGS.hpp>

#include <ngs/ReadCollection.hpp>
#include <ngs/ReadIterator.hpp>

#include <benchmark/benchmark.h>

#include <cstdint>
#include <functional>
#include <string>
#include <thread>
#include <vector>
#include <iostream>

namespace
{

std::string g_sraPath;

// Results are private to each worker thread.
// This avoids synchronization/atomics in the hot processing loop.
struct Result
{
    std::uint64_t reads = 0;
    std::uint64_t bases = 0;
    std::uint64_t qualityBytes = 0;
};

// Process one contiguous range of reads.
//
// Each worker opens its own ReadCollection and gets its own iterator.
// This avoids sharing an NGS iterator between threads and therefore
// avoids requiring a mutex around nextRead().
void worker(
    const std::string& path,
    std::uint64_t first,
    std::uint64_t count,
    Result& result)
{
    // Open the SRA dataset through the NGS/VDB layer. We're not loading the complete dataset into memory.
    auto run = ncbi::NGS::openReadCollection(path);

    // Create an "iterator" over this worker's assigned range of reads.
    auto reads = run.getReadRange(first, count, ngs::Read::all);

    // Sequentially process every read in this thread's range.
    while (reads.nextRead()) {
        // Retrieve/decode the DNA sequence and quality scores for the current read.
        const auto bases = reads.getReadBases();
        const auto qualities = reads.getReadQualities();

        ++result.reads;
        result.bases += bases.size();
        result.qualityBytes += qualities.size();
    }
}

void BM_SraRead(benchmark::State& state)
{
    const auto threadCount =
        static_cast<unsigned>(state.range(0));

    // Open once outside the measured loop to obtain the total number
    // of reads that need to be divided among the workers.
    auto run = ncbi::NGS::openReadCollection(g_sraPath);

    const std::uint64_t totalReads = run.getReadCount(ngs::Read::all);

    // Basic number of reads assigned to each thread.
    const std::uint64_t baseChunk = totalReads / threadCount;

    // If totalReads is not evenly divisible by threadCount,
    // distribute the remaining reads among the first workers.
    const std::uint64_t remainder = totalReads % threadCount;

    std::uint64_t processedReads = 0;
    std::uint64_t processedBases = 0;
    std::uint64_t processedQualities = 0;

    for (auto _ : state) {
        std::vector<Result> results(threadCount);

        {
            // std::jthread automatically joins when destroyed.
            // The scope therefore acts as our synchronization point:
            // after leaving it, all workers are guaranteed to be done.
            std::vector<std::jthread> threads;
            threads.reserve(threadCount);

            std::uint64_t first = 1;

            for (unsigned i = 0; i < threadCount; ++i) {
                const std::uint64_t count = baseChunk + (i < remainder ? 1 : 0);

                threads.emplace_back(
                    worker,
                    std::cref(g_sraPath),
                    first,
                    count,
                    std::ref(results[i]));

                first += count;
            }

        } // all std::jthreads join here

        // No synchronization is needed here because every worker
        // has finished and each Result was owned by one worker.
        processedReads = 0;
        processedBases = 0;
        processedQualities = 0;

        for (const auto& result : results) {
            processedReads += result.reads;
            processedBases += result.bases;
            processedQualities += result.qualityBytes;
        }

        // Make the accumulated results observable to the optimizer.
        benchmark::DoNotOptimize(processedReads);
        benchmark::DoNotOptimize(processedBases);
        benchmark::DoNotOptimize(processedQualities);
    }

    // Report throughput. kIsRate tells Google Benchmark to divide
    // these values by the measured benchmark time.
    state.counters["Mreads/s"] = benchmark::Counter(static_cast<double>(processedReads) / 1'000'000.0, benchmark::Counter::kIsRate);

    state.counters["Mbases/s"] = benchmark::Counter(static_cast<double>(processedBases) / 1'000'000.0, benchmark::Counter::kIsRate);
}

BENCHMARK(BM_SraRead)
    ->Arg(1)
    ->Arg(2)
    ->Arg(4)
    ->Arg(8)
    ->Arg(16)

    // One iteration means one complete scan of the SRA dataset.
    // Repetitions are controlled from the command line.
    ->Iterations(1)

    // The work happens in worker threads. Wall-clock time is therefore
    // the meaningful measurement rather than the benchmark thread's
    // CPU time.
    ->UseRealTime()

    ->Unit(benchmark::kMillisecond);

} // namespace

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <SRA file> [benchmark options]\n";
        return 1;
    }

    g_sraPath = argv[1];

    // Remove our SRA path argument before passing the command line to Google Benchmark.
    for (int i = 1; i < argc - 1; ++i)
        argv[i] = argv[i + 1];

    --argc;

    benchmark::Initialize(&argc, argv);

    if (benchmark::ReportUnrecognizedArguments(argc, argv))
        return 1;

    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();

    return 0;
}