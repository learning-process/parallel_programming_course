@echo off

build\bin\core_func_tests.exe --gtest_also_run_disabled_tests --gtest_repeat=10 --gtest_recreate_environments_when_repeating || exit 1
build\bin\ref_func_tests.exe  --gtest_also_run_disabled_tests --gtest_repeat=10 --gtest_recreate_environments_when_repeating || exit 1

if "%CLANG_BUILD%" NEQ "1" mpiexec.exe -np 4 build\bin\sample_mpi.exe
if "%CLANG_BUILD%" NEQ "1" mpiexec.exe -np 4 build\bin\sample_mpi_boost.exe
if "%CLANG_BUILD%" NEQ "1" build\bin\sample_omp.exe
build\bin\sample_stl.exe
build\bin\sample_tbb.exe

if "%CLANG_BUILD%" NEQ "1" mpiexec.exe -np 4 build\bin\mpi_func_tests.exe --gtest_repeat=10 || exit 1
if "%CLANG_BUILD%" NEQ "1" build\bin\mpi_func_tests.exe || exit 1
if "%CLANG_BUILD%" NEQ "1" build\bin\omp_func_tests.exe  --gtest_also_run_disabled_tests --gtest_repeat=10 --gtest_recreate_environments_when_repeating || exit 1
build\bin\seq_func_tests.exe  --gtest_also_run_disabled_tests --gtest_repeat=10 --gtest_recreate_environments_when_repeating || exit 1
build\bin\stl_func_tests.exe  --gtest_also_run_disabled_tests --gtest_repeat=10 --gtest_recreate_environments_when_repeating || exit 1
build\bin\tbb_func_tests.exe  --gtest_also_run_disabled_tests --gtest_repeat=10 --gtest_recreate_environments_when_repeating || exit 1
