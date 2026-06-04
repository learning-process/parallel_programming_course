# Thread Task oneTBB Implementation

Fill in this report with the oneTBB implementation for the thread-based task. Compare it with
`../seq/report.md`.

## 4.4 oneTBB Implementation

- Source files: `<oneTBB source files>`
- Source files to describe: `include/ops_tbb.hpp`, `src/ops_tbb.cpp`
- Task class to describe: `NesterovATestTaskTBB`
- Used oneTBB algorithms: `<parallel_for / parallel_reduce / flow graph / other algorithms>`
- oneTBB algorithm to explain: `tbb::parallel_for`
- Blocked ranges / partitioning: `<Range and partitioning strategy>`
- Reductions, if used: `<Reduction strategy>`
- Task-based decomposition: `<Task decomposition strategy>`
- Result aggregation: `<How partial task results are combined>`
- Synchronization primitive to explain: `std::atomic<int>`
- Methods to describe: `ValidationImpl`, `PreProcessingImpl`, `RunImpl`, `PostProcessingImpl`
- Limitations: `<Known limitations>`
