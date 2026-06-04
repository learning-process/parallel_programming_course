# Thread-Based Task Report

Fill in this report for a thread-based semester or assignment. It aggregates the sequential, OpenMP, oneTBB,
STL, and combined thread-level reports. Skip implementations that are not required for your assignment.

Child reports:

- `seq/report.md`
- `omp/report.md`
- `tbb/report.md`
- `stl/report.md`
- `all/report.md`

## Where to Write Thread-Semester Details

| Repository path               | What to describe                               |
| ----------------------------- | ---------------------------------------------- |
| `seq/include`, `seq/src`      | Sequential baseline implementation             |
| `omp/include`, `omp/src`      | OpenMP implementation                          |
| `tbb/include`, `tbb/src`      | oneTBB implementation                          |
| `stl/include`, `stl/src`      | STL or `std::thread` implementation            |
| `all/include`, `all/src`      | Combined implementation                        |
| `tests/functional/main.cpp`   | Functional tests using `data/pic.ppm`          |
| `tests/performance/main.cpp`  | Performance tests for all thread variants      |

## Problem Statement

Describe the thread-based task.

- What must be implemented: `<Description of the required computation>`
- Input data: `<Input data format, source, size, and meaning>`
- Output data: `<Output data format and meaning>`
- Constraints: `<Input limits, memory limits, time limits, or domain-specific restrictions>`
- Correctness criteria: `<Rules used to decide that the output is correct>`
- Expected behavior: `<Expected behavior for normal, edge, and invalid inputs>`

## Algorithm Description

Explain the algorithm used by the thread-based implementations.

- General idea of the algorithm: `<Algorithm overview>`
- Sequential algorithm: `<Step-by-step sequential baseline>`
- Parallel decomposition: `<How work is split between threads>`
- Data partitioning: `<How input data is divided between threads>`
- Synchronization strategy: `<Barriers, locks, reductions, atomics, or none>`
- Result aggregation: `<How partial thread results are combined>`
- Possible data races and how they are avoided: `<Race risks and prevention strategy>`
- Complexity, if applicable: `<Time and memory complexity>`

## Build and Run Instructions

Provide the commands used for the thread-based task.

```bash
<Configure command>
<Build command>
<Test command>
<Run command>
```

- Command-line arguments: `<Command-line arguments>`

## Correctness Verification

Explain how thread implementations were checked against the sequential baseline.

- Implemented tests: `<Thread functional tests and performance tests>`
- Normal cases: `<Normal cases>`
- Edge cases: `<Edge cases>`
- Invalid input cases, if applicable: `<Invalid input cases>`
- Comparison with sequential implementation: `<How thread outputs were compared with sequential output>`
- Floating-point tolerance, if applicable: `<Tolerance value and reason>`
- Correctness explanation: `<How correctness was checked>`
- Test input data: `data/pic.ppm` is loaded in functional tests through `stb_image`
- Functional test class to mention: `NesterovARunFuncTestsThreads`
- Performance test class to mention: `ExampleRunPerfTestThreads`
- Implementation classes to describe: `NesterovATestTaskSEQ`, `NesterovATestTaskOMP`, `NesterovATestTaskTBB`,
  `NesterovATestTaskSTL`, `NesterovATestTaskALL`

| Test case     | Input size / parameters | Expected result | Actual result | Status            |
| ------------- | ----------------------: | --------------- | ------------- | ----------------- |
| `<Test name>` | `<Input>`               | `<Expected>`    | `<Actual>`    | `<Passed/Failed>` |

## Experiment Setup

Describe the environment used for thread performance measurements.

| Parameter               | Value                    |
| ----------------------- | ------------------------ |
| CPU                     | `<CPU model>`            |
| RAM                     | `<RAM size>`             |
| OS                      | `<Operating system>`     |
| Compiler                | `<Compiler and version>` |
| Build type              | `<Debug/Release>`        |
| CMake options           | `<CMake options>`        |
| Number of runs          | `<N>`                    |
| Input data sizes        | `<Input data sizes>`     |
| Number of threads       | `<Thread counts>`        |
| Time measurement method | `<Method>`               |

## Performance Results

Use the same input data for every implementation.

### Execution Time

| Implementation | Input size | Processes | Threads | Time, ms | Notes     |
| -------------- | ---------: | --------: | ------: | -------: | --------- |
| Sequential     | `<N>`      | 1         | 1       | `<Time>` | `<Notes>` |
| OpenMP         | `<N>`      | 1         | `<T>`   | `<Time>` | `<Notes>` |
| oneTBB         | `<N>`      | 1         | `<T>`   | `<Time>` | `<Notes>` |
| STL            | `<N>`      | 1         | `<T>`   | `<Time>` | `<Notes>` |
| Combined       | `<N>`      | `<P>`     | `<T>`   | `<Time>` | `<Notes>` |

### Speedup

| Implementation     | Input size | Threads | Sequential time, ms | Parallel time, ms | Speedup     |
| ------------------ | ---------: | ------: | ------------------: | ----------------: | ----------: |
| `<Implementation>` | `<N>`      | `<T>`   | `<Seq time>`        | `<Par time>`      | `<Speedup>` |

### Efficiency

| Implementation     | Input size | Threads | Speedup     | Efficiency     |
| ------------------ | ---------: | ------: | ----------: | -------------: |
| `<Implementation>` | `<N>`      | `<T>`   | `<Speedup>` | `<Efficiency>` |

### Best Result Summary

| Implementation     | Best configuration | Best time, ms | Speedup     | Comment     |
| ------------------ | ------------------ | ------------: | ----------: | ----------- |
| `<Implementation>` | `<Configuration>`  | `<Time>`      | `<Speedup>` | `<Comment>` |

```text
Speedup = Sequential time / Parallel time
Efficiency = Speedup / Number of workers
```

For thread-based implementations, workers usually mean the number of threads.

## Results Analysis

- Which thread implementation showed the best performance?
- Why did this implementation perform better?
- Where was speedup not achieved?
- What overheads affected the result?
- How did the number of threads affect performance?
- Were there any anomalous results?
- How can the implementation be improved?

## Conclusion

- Short summary: `<Summary of completed work>`
- Best implementation: `<Best implementation>`
- Explanation of the best result: `<Why this result was best>`
- Limitations: `<Current limitations>`
- Possible future improvements: `<Future improvements>`
