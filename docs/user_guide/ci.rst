Continuous Integration (CI)
============================

Students need to pass all the checks in the CI pipeline before their work can be considered for submission.
This includes successful code checkout, build ans testing stages.
Each integration is verified by an automated build and automated tests.

CI Pipeline
------------

The CI pipeline for this project is illustrated in the following diagram:

.. image:: ../_static/ci_graph.svg
    :alt: CI Pipeline Diagram
    :align: center

Running ``scripts/run_tests.py``
--------------------------------

Automated tests are executed through the ``scripts/run_tests.py`` helper.  The
script requires several environment variables to be defined:

``PPC_NUM_THREADS``
    Number of threads to use.  The value is also exported as
    ``OMP_NUM_THREADS``.

``PPC_NUM_PROC``
    Number of MPI processes to launch.

``PPC_ASAN_RUN``
    Set to ``1`` when sanitizers are enabled to skip ``valgrind`` runs (optional,
    default ``0``).

``PPC_IGNORE_TEST_TIME_LIMIT``
    Set to ``1`` to disable test time limits (optional, default ``0``).

The execution mode is selected with ``--running-type``.  The most common modes
are ``threads`` for shared-memory backends and ``processes`` for MPI based
tests.  ``performance`` mode runs performance benchmarks.

Example usage:

.. code-block:: bash

   export PPC_NUM_THREADS=4
   export PPC_NUM_PROC=2

   # Multithreaded functional tests
   scripts/run_tests.py --running-type="threads"

   # MPI functional tests
   scripts/run_tests.py --running-type="processes"

   # Performance benchmarks
   scripts/run_tests.py --running-type="performance"

Additional MPI arguments can be supplied with ``--additional-mpi-args`` when
running in ``processes`` mode.

The ``--counts`` option allows sequential execution of tests with several
thread/process counts.  When specified, the script will iterate over the provided
values, updating ``PPC_NUM_THREADS`` or ``PPC_NUM_PROC`` accordingly before each
run.

Use ``--verbose`` to print every command executed by ``run_tests.py``.  This can
be helpful for debugging CI failures or verifying the exact arguments passed to
the test binaries.
