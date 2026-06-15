Build the Project with ``CMake``
================================

Navigate to the project root.

1. **Configure the build**: ``Makefile``, ``.sln``, etc.

   .. code-block:: bash

      cmake -S . -B build

   To configure only selected tasks:

   .. code-block:: bash

      cmake -S . -B build -DPPC_TASKS="example"

   Optional: enable sanitizers for local debugging

   .. code-block:: bash

      cmake -S . -B build -D ENABLE_ADDRESS_SANITIZER=ON -D CMAKE_BUILD_TYPE=RelWithDebInfo

   *Help on CMake keys:*


   - ``-D USE_FUNC_TESTS=ON`` enable functional tests.
   - ``-D USE_PERF_TESTS=ON`` enable performance tests.
   - ``-D PPC_BUILD_COMPONENTS=OFF`` skip C++ modules, tasks, tests, and
     their dependencies, useful for docs-only or scoreboard-only configurations.
   - ``-D PPC_TASKS=all`` builds every task (default). Pass one task or a semicolon list,
     for example ``-D PPC_TASKS="example"``, to limit the build.
   - ``-D PPC_IMPLEMENTATIONS="seq;omp"`` select implementation folders to
     configure.
   - ``-D CMAKE_BUILD_TYPE=Release`` normal build (default).
   - ``-D CMAKE_BUILD_TYPE=RelWithDebInfo`` recommended when using sanitizers or
     running ``valgrind`` to keep debug information.
   - ``-D CMAKE_BUILD_TYPE=Debug`` for debugging sessions.

   *A corresponding flag can be omitted if it's not needed.*

   OSH is part of the process-technology set on Linux. It is built by default
   from the shallow ``3rdparty/openmpi`` source backend together with MPI. A
   focused example build can be configured as:

   .. code-block:: bash

      git submodule update --init --recursive
      cmake -S . -B build-osh -DPPC_BUILD_COMPONENTS=ON -DPPC_TASKS=example
      cmake --build build-osh --parallel

2. **Build the project**:

   .. code-block:: bash

      cmake --build build --config Release --parallel

3. **Run tests**:

   Prefer the helper runner described in ``User Guide → CI``.

   OSH example on Linux:

   .. code-block:: bash

      PPC_NUM_THREADS=1 scripts/run_tests.py --running-type=processes --counts 2 --build-dir build-osh
      PPC_NUM_THREADS=1 PPC_NUM_PROC=2 ./build-osh/ppc_openmpi/install/bin/oshrun -np 2 ./build-osh/bin/ppc_func_tests --gtest_filter="*_osh_*:*OSHEnabled*"
