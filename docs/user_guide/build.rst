Build the Project with ``CMake``
================================

Navigate to the project root.

1. **Configure the build**: ``Makefile``, ``.sln``, etc.

   .. code-block:: bash

      cmake -S . -B build -D USE_FUNC_TESTS=ON -D USE_PERF_TESTS=ON -D CMAKE_BUILD_TYPE=Release

   Optional: enable sanitizers for local debugging

   .. code-block:: bash

      cmake -S . -B build -D ENABLE_ADDRESS_SANITIZER=ON -D CMAKE_BUILD_TYPE=RelWithDebInfo

   *Help on CMake keys:*


   - ``-D USE_FUNC_TESTS=ON`` enable functional tests.
   - ``-D USE_PERF_TESTS=ON`` enable performance tests.
   - ``-D CMAKE_BUILD_TYPE=Release`` normal build (default).
   - ``-D CMAKE_BUILD_TYPE=RelWithDebInfo`` recommended when using sanitizers or
     running ``valgrind`` to keep debug information.
   - ``-D CMAKE_BUILD_TYPE=Debug`` for debugging sessions.

   *A corresponding flag can be omitted if it's not needed.*

2. **Build the project**:

   .. code-block:: bash

      cmake --build build --config Release --parallel

3. **Run tests**:

   Prefer the helper runner described in ``User Guide → CI``.
