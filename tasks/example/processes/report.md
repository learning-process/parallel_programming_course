# Process-Based Task Report

Fill in this report for an MPI/process semester or assignment. It aggregates process tasks T1, T2, and T3.
Fill in only the tasks required for your assignment. Each task must compare its MPI implementation only with its
own sequential baseline.

Child reports:

- `t1/report.md`
- `t2/report.md`
- `t3/report.md`

## Where to Write Process-Semester Details

| Repository path                         | What to describe                              |
| --------------------------------------- | --------------------------------------------- |
| `t1/seq/include`, `t1/seq/src`          | Sequential baseline for process task T1       |
| `t1/mpi/include`, `t1/mpi/src`          | MPI implementation for process task T1        |
| `t1/tests/functional/main.cpp`          | Functional tests for process task T1          |
| `t1/tests/performance/main.cpp`         | Performance tests for process task T1         |
| `t2/seq/include`, `t2/seq/src`          | Sequential baseline for process task T2       |
| `t2/mpi/include`, `t2/mpi/src`          | MPI implementation for process task T2        |
| `t2/tests/functional/main.cpp`          | Functional tests for process task T2          |
| `t2/tests/performance/main.cpp`         | Performance tests for process task T2         |
| `t3/seq/include`, `t3/seq/src`          | Sequential baseline for process task T3       |
| `t3/mpi/include`, `t3/mpi/src`          | MPI implementation for process task T3        |
| `t3/tests/functional/main.cpp`          | Functional tests for process task T3          |
| `t3/tests/performance/main.cpp`         | Performance tests for process task T3         |

Each `t1`, `t2`, and `t3` report owns its own correctness and performance baseline. Do not compare an MPI task
with a sequential implementation from another process task or from the thread branch.

## 2. Problem Statement

Describe the process-based assignment scope.

- Required process tasks: `<T1 / T2 / T3>`
- Input data: `<Input data format, source, size, and meaning>`
- Output data: `<Output data format and meaning>`
- Constraints: `<Input limits, memory limits, process limits, or memory limits>`
- Correctness criteria: `<Rules used to decide that the output is correct>`
- Expected behavior: `<Expected behavior for normal, edge, and invalid inputs>`

## 5. Build and Run Instructions

Provide the commands used for process-based tasks.

```bash
<Configure command>
<Build command>
<Test command>
<Run command>
```

- Command-line arguments: `<Command-line arguments>`

## 7. Experiment Setup

Describe the environment used for MPI/process performance measurements.

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
| Number of processes     | `<Process counts>`       |
| Time measurement method | `<Method>`               |

## 8. Performance Results

Use a separate sequential baseline for each process task.

### 8.1 Execution Time

| Task   | Implementation | Input size | Processes | Threads | Time, ms | Notes     |
| ------ | -------------- | ---------: | --------: | ------: | -------: | --------- |
| `<T1>` | Sequential     | `<N>`      | 1         | 1       | `<Time>` | `<Notes>` |
| `<T1>` | MPI            | `<N>`      | `<P>`     | 1       | `<Time>` | `<Notes>` |
| `<T2>` | Sequential     | `<N>`      | 1         | 1       | `<Time>` | `<Notes>` |
| `<T2>` | MPI            | `<N>`      | `<P>`     | 1       | `<Time>` | `<Notes>` |
| `<T3>` | Sequential     | `<N>`      | 1         | 1       | `<Time>` | `<Notes>` |
| `<T3>` | MPI            | `<N>`      | `<P>`     | 1       | `<Time>` | `<Notes>` |

### 8.2 Speedup

| Task     | Input size | Processes | Sequential time, ms | Parallel time, ms | Speedup     |
| -------- | ---------: | --------: | ------------------: | ----------------: | ----------: |
| `<Task>` | `<N>`      | `<P>`     | `<Seq time>`        | `<Par time>`      | `<Speedup>` |

### 8.3 Efficiency

| Task     | Input size | Processes | Speedup     | Efficiency     |
| -------- | ---------: | --------: | ----------: | -------------: |
| `<Task>` | `<N>`      | `<P>`     | `<Speedup>` | `<Efficiency>` |

### 8.4 Best Result Summary

| Task     | Best configuration | Best time, ms | Speedup     | Comment     |
| -------- | ------------------ | ------------: | ----------: | ----------- |
| `<Task>` | `<Configuration>`  | `<Time>`      | `<Speedup>` | `<Comment>` |

```text
Speedup = Sequential time / Parallel time
Efficiency = Speedup / Number of workers
```

For process-based implementations, workers usually mean the number of MPI processes.

## 9. Results Analysis

- Which process task showed the best MPI speedup?
- Why did this task perform better?
- Where was speedup not achieved?
- What MPI communication overheads affected the result?
- How did the number of processes affect performance?
- Were there any anomalous results?
- How can the implementation be improved?

## 10. Conclusion

- Short summary: `<Summary of completed process tasks>`
- Best implementation: `<Best task and configuration>`
- Explanation of the best result: `<Why this result was best>`
- Limitations: `<Current limitations>`
- Possible future improvements: `<Future improvements>`
