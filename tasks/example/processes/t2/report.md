# Process Task T2 Report

Fill in this report for the second process-based task. Compare `mpi/report.md` only with `seq/report.md` from this
same `t2` directory.

Child reports:

- `seq/report.md`
- `mpi/report.md`

## 2. Problem Statement

- What must be implemented: `<T2 required computation>`
- Input data: `<T2 input data>`
- Output data: `<T2 output data>`
- Constraints: `<T2 constraints>`
- Correctness criteria: `<T2 correctness criteria>`
- Expected behavior: `<T2 expected behavior>`

## 3. Algorithm Description

- General idea of the algorithm: `<T2 algorithm overview>`
- Sequential algorithm: `<T2 sequential baseline>`
- Parallel decomposition: `<How T2 work is split between processes>`
- Data partitioning: `<How T2 data is divided>`
- Synchronization strategy: `<MPI synchronization strategy>`
- Result aggregation: `<How T2 partial results are combined>`
- Possible data races and how they are avoided: `N/A for separate MPI process memory unless threads are used`
- Complexity, if applicable: `<T2 time and memory complexity>`

## 6. Correctness Verification

- Implemented tests: `<T2 tests>`
- Normal cases: `<T2 normal cases>`
- Edge cases: `<T2 edge cases>`
- Invalid input cases, if applicable: `<T2 invalid input cases>`
- Comparison with sequential implementation: `<How T2 MPI output is compared with T2 sequential output>`
- Floating-point tolerance, if applicable: `<Tolerance value and reason>`
- Correctness explanation: `<How T2 correctness was checked>`
- Test input data: `data/pic.ppm` is loaded in functional tests through `stb_image`
- Functional test class to mention: `NesterovARunFuncTestsProcesses2`
- Performance test class to mention: `ExampleRunPerfTestProcesses2`
- Implementation classes to describe: `NesterovATestTaskSEQ`, `NesterovATestTaskMPI`

| Test case     | Input size / parameters | Expected result | Actual result | Status            |
| ------------- | ----------------------: | --------------- | ------------- | ----------------- |
| `<Test name>` | `<Input>`               | `<Expected>`    | `<Actual>`    | `<Passed/Failed>` |

## 8. Performance Results

| Implementation | Input size | Processes | Threads | Time, ms | Notes     |
| -------------- | ---------: | --------: | ------: | -------: | --------- |
| Sequential     | `<N>`      | 1         | 1       | `<Time>` | `<Notes>` |
| MPI            | `<N>`      | `<P>`     | 1       | `<Time>` | `<Notes>` |
