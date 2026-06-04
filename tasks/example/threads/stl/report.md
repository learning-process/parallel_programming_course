# Thread Task STL / std::thread Implementation

Fill in this report with the STL or `std::thread` implementation for the thread-based task. Compare it with
`../seq/report.md`.

- Source files: `<STL or std::thread source files>`
- Source files to describe: `include/ops_stl.hpp`, `src/ops_stl.cpp`
- Task class to describe: `NesterovATestTaskSTL`
- Number of threads: `<Number of threads>`
- Manual thread partitioning: `<How work is split manually>`
- Thread primitive to explain: `std::thread`
- Synchronization primitives: `<mutex / atomic / condition_variable / other primitives>`
- Synchronization primitive to explain: `std::atomic<int>`
- Result aggregation: `<How thread results are combined>`
- Methods to describe: `ValidationImpl`, `PreProcessingImpl`, `RunImpl`, `PostProcessingImpl`
- Limitations: `<Known limitations>`
