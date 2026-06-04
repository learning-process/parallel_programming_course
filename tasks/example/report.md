# Parallel Programming Task Report Template

This is the root report template for course tasks. The complete template is split across this report tree.
Fill in only the branch that matches your semester or assignment:

- use `threads/report.md` for thread-based tasks;
- use `processes/report.md` for MPI/process tasks;
- use implementation-level reports for the concrete technologies that you implemented.

Students normally complete either the thread-based semester or the process/MPI semester. Do not describe both
groups unless the assignment explicitly requires both.

## General Information

Fill in this table for the whole task report.

| Field                      | Value                                                    |
| -------------------------- | -------------------------------------------------------- |
| Student full name          | `<Student full name>`                                    |
| Group                      | `<Group>`                                                |
| Task name                  | `<Task name>`                                            |
| Task path in repository    | `<Path to task>`                                         |
| Implementation variants    | `<Sequential / MPI / OpenMP / oneTBB / STL / Combined>`  |
| Used technologies          | `<Used technologies>`                                    |
| Repository branch / commit | `<Branch or commit hash>`                                |

## How to Use the Report Tree

Use the existing reports below instead of creating new Markdown files.

For a single PDF report, assemble `report.md` first, then exactly one semester branch unless the assignment
explicitly requires both branches. Include only the implementation-level reports that match the completed work.
A full course template PDF may include both branches for reference, but a student submission should normally keep
only the branch assigned for the semester.

Recommended PDF order for a thread-based semester:

1. `report.md`
2. `threads/report.md`
3. `threads/seq/report.md`
4. `threads/omp/report.md`
5. `threads/tbb/report.md`
6. `threads/stl/report.md`
7. `threads/all/report.md`

Recommended PDF order for an MPI/process semester:

1. `report.md`
2. `processes/report.md`
3. `processes/t1/report.md`
4. `processes/t1/seq/report.md`
5. `processes/t1/mpi/report.md`
6. `processes/t2/report.md`
7. `processes/t2/seq/report.md`
8. `processes/t2/mpi/report.md`
9. `processes/t3/report.md`
10. `processes/t3/seq/report.md`
11. `processes/t3/mpi/report.md`

| Report file                     | What to fill in                                     |
| ------------------------------- | --------------------------------------------------- |
| `threads/report.md`             | Thread-semester task overview and thread results    |
| `threads/seq/report.md`         | Sequential baseline for the thread task             |
| `threads/omp/report.md`         | OpenMP implementation details                       |
| `threads/tbb/report.md`         | oneTBB implementation details                       |
| `threads/stl/report.md`         | STL or `std::thread` implementation details         |
| `threads/all/report.md`         | Combined implementation details                     |
| `processes/report.md`           | Process-semester overview and process results       |
| `processes/t1/report.md`        | Process task T1 statement, verification, and result |
| `processes/t1/seq/report.md`    | Sequential baseline for process task T1             |
| `processes/t1/mpi/report.md`    | MPI implementation for process task T1              |
| `processes/t2/report.md`        | Process task T2 statement, verification, and result |
| `processes/t2/seq/report.md`    | Sequential baseline for process task T2             |
| `processes/t2/mpi/report.md`    | MPI implementation for process task T2              |
| `processes/t3/report.md`        | Process task T3 statement, verification, and result |
| `processes/t3/seq/report.md`    | Sequential baseline for process task T3             |
| `processes/t3/mpi/report.md`    | MPI implementation for process task T3              |

## Shared Files to Mention When Relevant

Mention these shared files when they affect your task, tests, or measurements.

| Repository path                    | What to mention in the report                              |
| ---------------------------------- | ---------------------------------------------------------- |
| `common/include/common.hpp`        | Defines shared `InType`, `OutType`, `TestType`, `BaseTask` |
| `data/pic.ppm`                     | Shared image fixture used by functional tests              |
| `settings.json`                    | Enables or disables example implementations                |
| `info.json`                        | Stores student metadata for automation                     |
| `threads/tests/functional`         | Functional tests for thread implementations                |
| `threads/tests/performance`        | Performance tests for thread implementations               |
| `processes/t*/tests/functional`    | Functional tests for each process task                     |
| `processes/t*/tests/performance`   | Performance tests for each process task                    |

## Where to Describe Implementations

Write implementation details in the file that matches the implementation you completed.

- Sequential implementation: `threads/seq/report.md` or `processes/t*/seq/report.md`
- MPI / processes implementation: `processes/t*/mpi/report.md`
- OpenMP implementation: `threads/omp/report.md`
- oneTBB implementation: `threads/tbb/report.md`
- STL / `std::thread` implementation: `threads/stl/report.md`
- Combined implementation: `threads/all/report.md`

## Performance Formulas

Use these formulas in the relevant semester report:

```text
Speedup = Sequential time / Parallel time
Efficiency = Speedup / Number of workers
```

Workers can mean the number of threads, the number of processes, or a combined process/thread configuration
depending on the implementation.

## Appendix

Use this root appendix for shared material that applies to the whole task.

- Additional logs: `<Additional logs>`
- Raw benchmark output: `<Raw benchmark output>`
- Links to source files: `<Links to source files>`
- Screenshots, if needed: `<Screenshots>`

## How to Fill in This Report

- Do not write only "it became faster" without measurements.
- Always compare parallel implementations with the sequential implementation.
- Always specify input data and launch parameters.
- Explain bad, unexpected, or unstable results.
- The report must be reproducible.
- Do not invent performance numbers.
- Use the same input data for fair comparison.
- Use Release builds for performance measurements unless explicitly stated otherwise.
- Fill in only the implementations required by your semester or assignment.
- Mark non-applicable sections as `N/A` if your instructor requires the section to remain visible.
