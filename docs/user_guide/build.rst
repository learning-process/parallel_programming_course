Build the Project with ``CMake``
================================

Navigate to a source code folder.

1. **Configure the build**: ``Makefile``, ``.sln``, etc.

   .. code-block:: bash

      mkdir build && cd build
      cmake -D USE_SEQ=ON -D USE_MPI=ON -D USE_OMP=ON -D USE_TBB=ON -D USE_STL=ON -D USE_FUNC_TESTS=ON -D USE_PERF_TESTS=ON -D CMAKE_BUILD_TYPE=Release ..

   *Help on CMake keys:*

   - ``-D USE_SEQ=ON`` enable ``Sequential`` labs (based on OpenMP's CMakeLists.txt).
   - ``-D USE_MPI=ON`` enable ``MPI`` labs.
   - ``-D USE_OMP=ON`` enable ``OpenMP`` labs.
   - ``-D USE_TBB=ON`` enable ``TBB`` labs.
   - ``-D USE_STL=ON`` enable ``std::thread`` labs.
   - ``-D USE_FUNC_TESTS=ON`` enable functional tests.
   - ``-D USE_PERF_TESTS=ON`` enable performance tests.
   - ``-D CMAKE_BUILD_TYPE=Release`` required parameter for stable work of repo.

   *A corresponding flag can be omitted if it's not needed.*

2. **Build the project**:

   .. code-block:: bash

      cmake --build . --config Release --parallel

3. **Check the task**:
   
   * Run ``<project's folder>/build/bin``
