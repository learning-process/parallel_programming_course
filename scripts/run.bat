@echo off

build\bin\core_func_tests.exe --gtest_also_run_disabled_tests --gtest_repeat=10 --gtest_recreate_environments_when_repeating || exit 1
build\bin\ref_func_tests.exe  --gtest_also_run_disabled_tests --gtest_repeat=10 --gtest_recreate_environments_when_repeating || exit 1
build\bin\omp_func_tests.exe  --gtest_also_run_disabled_tests --gtest_repeat=10 --gtest_recreate_environments_when_repeating || exit 1
build\bin\seq_func_tests.exe  --gtest_also_run_disabled_tests --gtest_repeat=10 --gtest_recreate_environments_when_repeating || exit 1
build\bin\tbb_func_tests.exe  --gtest_also_run_disabled_tests --gtest_repeat=10 --gtest_recreate_environments_when_repeating || exit 1

for /r "." %%a in (build\bin\*_mpi.exe) do (
    echo -------------------------------------
    echo %%~na
    echo -------------------------------------
    "C:\Program Files\Microsoft MPI\Bin\mpiexec.exe" -np 4 %%~fa --gtest_repeat=10 || exit 1
)

for /r "." %%a in (build\bin\*_stl.exe) do (
    echo -------------------------------------
    echo %%~na
    echo -------------------------------------
    %%~fa --gtest_also_run_disabled_tests --gtest_repeat=10 --gtest_recreate_environments_when_repeating || exit 1
)
