Environment Variables
=====================

The following environment variables can be used to configure the project's runtime behavior:

- ``PPC_NUM_PROC``: Specifies the number of processes to launch.
  Default: ``1``
  Can be queried from C++ with ``ppc::util::GetNumProc()``.

- ``PPC_NUM_THREADS``: Specifies the number of threads to use.
  Default: ``1``

- ``PPC_ASAN_RUN``: Specifies that application is compiler with sanitizers. Used by ``scripts/run_tests.py`` to skip ``valgrind`` runs.
  Default: ``0``

- ``PPC_IGNORE_TEST_TIME_LIMIT``: Specifies that test time limits are ignored. Used by ``scripts/run_tests.py`` to disable time limit enforcement.
  Default: ``0``
- ``PPC_TASK_MAX_TIME``: Maximum allowed execution time in seconds for functional tests.
  Default: ``1.0``
- ``PPC_PERF_MAX_TIME``: Maximum allowed execution time in seconds for performance tests.
  Default: ``10.0``
