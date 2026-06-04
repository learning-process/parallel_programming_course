# Thread Task OpenMP Implementation

Fill in this report with the OpenMP implementation for the thread-based task. Compare it with
`../seq/report.md`.

- Source files: `<OpenMP source files>`
- Source files to describe: `include/ops_omp.hpp`, `src/ops_omp.cpp`
- Task class to describe: `NesterovATestTaskOMP`
- Number of threads: `<Number of threads>`
- Parallel regions: `<Parallel regions used>`
- Parallel construct to explain: `#pragma omp parallel`
- Scheduling strategy: `<Static / dynamic / guided / default scheduling>`
- Reductions / critical sections / atomics, if used: `<Synchronization primitives>`
- Synchronization primitive to explain: `std::atomic<int>`
- Result aggregation: `<How partial thread results are combined>`
- Methods to describe: `ValidationImpl`, `PreProcessingImpl`, `RunImpl`, `PostProcessingImpl`
- Limitations: `<Known limitations>`
