Points
======

- For “process parallelism” semester

  - 1st MPI task

    +----------+
    | Solution |
    +==========+
    | 10       |
    +----------+

  - 2nd MPI task

    ======== ===========
    Solution Performance
    ======== ===========
    15       5
    ======== ===========

  - 3rd MPI task

    ======== ===========
    Solution Performance
    ======== ===========
    20       10
    ======== ===========

  - **Total : 60 points**

- For “thread parallelism” semester

  - Sequential version

    +----------+
    | Solution |
    +==========+
    | 4        |
    +----------+

  - OpenMP version

    ======== ===========
    Solution Performance
    ======== ===========
    6        3
    ======== ===========

  - TBB version

    ======== ===========
    Solution Performance
    ======== ===========
    6        3
    ======== ===========

  - std::thread version

    ======== ===========
    Solution Performance
    ======== ===========
    8        6
    ======== ===========

  - “MPI + threads” version (The threading technology is chosen
    randomly)

    ======== ===========
    Solution Performance
    ======== ===========
    10       8
    ======== ===========

  - **Total : 54 points**

- The rule for earning performance points.

  The ratio of efficiency percentage to points percentage, where the maximum points is 100% and the minimum points is 0%.

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

- Report

  +--------------+--------------+--------------------+-------+
  | Completeness | Text Quality | Formatting Quality | Total |
  +==============+==============+====================+=======+
  | 5            | 2.5          | 2.5                | 10    |
  +--------------+--------------+--------------------+-------+

- Conversion of points into exam assessment or pass/fail

  - For 5-point grading system

    ============ =============== ============
    Points range Exam Assessment Student Pass
    ============ =============== ============
    [87, 100]    5               Passed
    [70, 87)     4               Passed
    [50, 70)     3               Passed
    < 50         2               Not Passed
    ============ =============== ============

  - For 7-point grading system **(our current system)**

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

- Penalties:

  - A deadline will be set for each version.
  - 1 point is deducted from the version’s score for each day of delay in submission.
  - The task is considered submitted when it is merged into the master/main branch.
  - The submission time is defined as the timestamp of the last commit that successfully passes the CI pipeline.

- Comments:

  - It is forbidden to write the report if all tasks are not completed.
  - Please keep in mind that one week before the end of the semester, the repository will be closed for final assessment.
