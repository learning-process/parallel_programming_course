Build the Project with ``CMake``
================================

Navigate to a source code folder.

1. **Configure the build**: ``Makefile``, ``.sln``, etc.

   .. code-block:: bash

      mkdir build && cd build
      cmake -D USE_FUNC_TESTS=ON -D USE_PERF_TESTS=ON -D CMAKE_BUILD_TYPE=Release ..

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

      cmake --build . --config Release --parallel

3. **Check the task**:
   
   * Run ``<project's folder>/build/bin``
