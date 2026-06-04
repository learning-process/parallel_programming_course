# Thread Task Combined Implementation

Fill in this report with the combined or `all` implementation. Use it only when the assignment requires combined
process/thread or multi-technology parallelism.

- Source files: `<Combined implementation source files>`
- Source files to describe: `include/ops_all.hpp`, `src/ops_all.cpp`
- Task class to describe: `NesterovATestTaskALL`
- Processes and threads configuration: `<Processes and threads per process>`
- Interaction between MPI and thread-level parallelism: `<How process-level and thread-level parallelism interact>`
- MPI calls to explain: `MPI_Comm_rank`, `MPI_Barrier`
- Data exchange: `<How data is exchanged between processes and threads>`
- Thread-level technology: `<OpenMP / oneTBB / STL / other technology>`
- Thread-level constructs to explain: `#pragma omp parallel`, `std::thread`, `tbb::parallel_for`
- Result aggregation: `<How final results are combined>`
- Synchronization primitive to explain: `std::atomic<int>`
- Methods to describe: `ValidationImpl`, `PreProcessingImpl`, `RunImpl`, `PostProcessingImpl`
- Limitations: `<Known limitations>`
