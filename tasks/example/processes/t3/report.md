# Process Task T3 Report

Fill in this report for the third process-based task. Compare `mpi/report.md` only with `seq/report.md` from this
same `t3` directory.

Child reports:

- `seq/report.md`
- `mpi/report.md`

## Problem Statement

- What must be implemented: `<T3 required computation>`
- Input data: `<T3 input data>`
- Output data: `<T3 output data>`
- Constraints: `<T3 constraints>`
- Correctness criteria: `<T3 correctness criteria>`
- Expected behavior: `<T3 expected behavior>`

## Algorithm Description

- General idea of the algorithm: `<T3 algorithm overview>`
- Sequential algorithm: `<T3 sequential baseline>`
- Parallel decomposition: `<How T3 work is split between processes>`
- Data partitioning: `<How T3 data is divided>`
- Synchronization strategy: `<MPI synchronization strategy>`
- Result aggregation: `<How T3 partial results are combined>`
- Possible data races and how they are avoided: `N/A for separate MPI process memory unless threads are used`
- Complexity, if applicable: `<T3 time and memory complexity>`

## Correctness Verification

- Implemented tests: `<T3 tests>`
- Normal cases: `<T3 normal cases>`
- Edge cases: `<T3 edge cases>`
- Invalid input cases, if applicable: `<T3 invalid input cases>`
- Comparison with sequential implementation: `<How T3 MPI output is compared with T3 sequential output>`
- Floating-point tolerance, if applicable: `<Tolerance value and reason>`
- Correctness explanation: `<How T3 correctness was checked>`
- Test input data: `data/pic.ppm` is loaded in functional tests through `stb_image`
- Functional test class to mention: `NesterovARunFuncTestsProcesses3`
- Performance test class to mention: `ExampleRunPerfTestProcesses3`
- Implementation classes to describe: `NesterovATestTaskSEQ`, `NesterovATestTaskMPI`

| Test case     | Input size / parameters | Expected result | Actual result | Status            |
| ------------- | ----------------------: | --------------- | ------------- | ----------------- |
| `<Test name>` | `<Input>`               | `<Expected>`    | `<Actual>`    | `<Passed/Failed>` |

## Performance Results

| Implementation | Input size | Processes | Threads | Time, ms | Notes     |
| -------------- | ---------: | --------: | ------: | -------: | --------- |
| Sequential     | `<N>`      | 1         | 1       | `<Time>` | `<Notes>` |
| MPI            | `<N>`      | `<P>`     | 1       | `<Time>` | `<Notes>` |
