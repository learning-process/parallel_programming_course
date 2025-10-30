Points
======

Overview
--------

- Total per semester (Practice): 70 points
- Report points are distributed per task and sum to 10

Processes semester (MPI)
------------------------

- Task 1 — Total: 12
  - Implementation: MPI 8 + Seq 2
  - Performance: 0
  - Report: 2

- Task 2 — Total: 23
  - Implementation: MPI 12 + Seq 3
  - Performance: 5
  - Report: 3

- Task 3 — Total: 35
  - Implementation: MPI 16 + Seq 4
  - Performance: 10
  - Report: 5

- Semester total: 70

Threads semester (OpenMP/TBB/std::thread)
-----------------------------------------

- seq — Total: 5 (S 4, R 1)
- omp — Total: 11 (S 6, A 3, R 2)
- tbb — Total: 11 (S 6, A 3, R 2)
- stl — Total: 16 (S 8, A 6, R 2)
- all — Total: 21 (S 10, A 8, R 3)

- Semester total: 64

Legend: S — Solution (implementation), A — Acceleration (performance), R — Report

Performance points mapping
--------------------------

The ratio of efficiency percentage to points percentage (max 100%, min 0%):

If the score is not an integer, it is rounded up.

  +----------------+-------------------+
  | Efficiency (%) | Points percentage |
  +================+===================+
  | >= 50%         | 100%              |
  +----------------+-------------------+
  | [45, 50)       | 90%               |
  +----------------+-------------------+
  | [42, 45)       | 80%               |
  +----------------+-------------------+
  | [40, 42)       | 70%               |
  +----------------+-------------------+
  | [37, 40)       | 60%               |
  +----------------+-------------------+
  | [35, 37)       | 50%               |
  +----------------+-------------------+
  | [32, 35)       | 40%               |
  +----------------+-------------------+
  | [30, 32)       | 30%               |
  +----------------+-------------------+
  | [27, 30)       | 20%               |
  +----------------+-------------------+
  | [25, 27)       | 10%               |
  +----------------+-------------------+
  | < 25%          | 0%                |
  +----------------+-------------------+

Report
------

+--------------+--------------+--------------------+-------+
| Completeness | Text Quality | Formatting Quality | Total |
+==============+==============+====================+=======+
| 5            | 2.5          | 2.5                | 10    |
+--------------+--------------+--------------------+-------+

Conversion to grade
-------------------

- 5-point scale

  ============ =============== ============
  Points range Exam Assessment Student Pass
  ============ =============== ============
  [92, 100]    5               Passed
  [70, 92)     4               Passed
  [50, 70)     3               Passed
  < 50         2               Not Passed
  ============ =============== ============

- 7-point scale (current)

  ============ =============== ============
  Points range Exam Assessment Student Pass
  ============ =============== ============
  [99, 100]    5.5             Passed
  [92, 99)     5               Passed
  [82, 92)     4.5             Passed
  [70, 82)     4               Passed
  [50, 70)     3               Passed
  < 50         2               Not Passed
  ============ =============== ============

Penalties
---------

- A deadline will be set for each task.
- 1 point is deducted from the version’s score for each day of delay.
- A task is considered accepted when it is merged into the default branch after a successful CI pipeline.
- The submission time is defined as the timestamp of the last meaningful commit (not an empty commit to re-trigger CI jobs).

Comments
--------

- One week before the end of the semester, the repository is closed for final assessment.
