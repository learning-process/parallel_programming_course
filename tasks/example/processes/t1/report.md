# Process Task T1 Report

Fill in this report for the first process-based task. Compare `mpi/report.md` only with `seq/report.md` from this
same `t1` directory.

Child reports:

- `seq/report.md`
- `mpi/report.md`

## 2. Problem Statement

- What must be implemented: `<T1 required computation>`
- Input data: `<T1 input data>`
- Output data: `<T1 output data>`
- Constraints: `<T1 constraints>`
- Correctness criteria: `<T1 correctness criteria>`
- Expected behavior: `<T1 expected behavior>`

## 3. Algorithm Description

- General idea of the algorithm: `<T1 algorithm overview>`
- Sequential algorithm: `<T1 sequential baseline>`
- Parallel decomposition: `<How T1 work is split between processes>`
- Data partitioning: `<How T1 data is divided>`
- Synchronization strategy: `<MPI synchronization strategy>`
- Result aggregation: `<How T1 partial results are combined>`
- Possible data races and how they are avoided: `N/A for separate MPI process memory unless threads are used`
- Complexity, if applicable: `<T1 time and memory complexity>`

## 6. Correctness Verification

- Implemented tests: `<T1 tests>`
- Normal cases: `<T1 normal cases>`
- Edge cases: `<T1 edge cases>`
- Invalid input cases, if applicable: `<T1 invalid input cases>`
- Comparison with sequential implementation: `<How T1 MPI output is compared with T1 sequential output>`
- Floating-point tolerance, if applicable: `<Tolerance value and reason>`
- Correctness explanation: `<How T1 correctness was checked>`
- Test input data: `data/pic.ppm` is loaded in functional tests through `stb_image`
- Functional test class to mention: `NesterovARunFuncTestsProcesses`
- Performance test class to mention: `ExampleRunPerfTestProcesses`
- Implementation classes to describe: `NesterovATestTaskSEQ`, `NesterovATestTaskMPI`

| Test case     | Input size / parameters | Expected result | Actual result | Status            |
| ------------- | ----------------------: | --------------- | ------------- | ----------------- |
| `<Test name>` | `<Input>`               | `<Expected>`    | `<Actual>`    | `<Passed/Failed>` |

## 8. Performance Results

| Implementation | Input size | Processes | Threads | Time, ms | Notes     |
| -------------- | ---------: | --------: | ------: | -------: | --------- |
| Sequential     | `<N>`      | 1         | 1       | `<Time>` | `<Notes>` |
| MPI            | `<N>`      | `<P>`     | 1       | `<Time>` | `<Notes>` |
