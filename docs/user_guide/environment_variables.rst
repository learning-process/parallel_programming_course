Environment Variables
=====================

The following environment variables can be used to configure the project's runtime behavior:

- ``PPC_NUM_PROC``: Specifies the number of processes to launch.
  Default: ``1``

- ``PPC_NUM_THREADS``: Specifies the number of threads to use.
  Default: ``1``

- ``PPC_ASAN_RUN``: Specifies that application is compiler with sanitizers. Used by ``scripts/run_tests.py`` to skip ``valgrind`` runs.
  Default: ``0``
