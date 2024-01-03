@echo off

for /r "." %%a in (build\bin\*_ref.exe) do (
    echo -------------------------------------
    echo %%~na
    echo -------------------------------------
    %%~fa --gtest_also_run_disabled_tests --gtest_repeat=10 --gtest_recreate_environments_when_repeating || exit 1
)

for /r "." %%a in (build\bin\*_mpi.exe) do (
    echo -------------------------------------
    echo %%~na
    echo -------------------------------------
    "C:\Program Files\Microsoft MPI\Bin\mpiexec.exe" -np 4 %%~fa --gtest_repeat=10 || exit 1
)

for /r "." %%a in (build\bin\*_seq.exe) do (
    echo -------------------------------------
    echo %%~na
    echo -------------------------------------
    %%~fa --gtest_also_run_disabled_tests --gtest_repeat=10 --gtest_recreate_environments_when_repeating || exit 1
)

for /r "." %%a in (build\bin\*_omp.exe) do (
    echo -------------------------------------
    echo %%~na
    echo -------------------------------------
    %%~fa --gtest_also_run_disabled_tests --gtest_repeat=10 --gtest_recreate_environments_when_repeating || exit 1
)

for /r "." %%a in (build\bin\*_tbb.exe) do (
    echo -------------------------------------
    echo %%~na
    echo -------------------------------------
    %%~fa --gtest_also_run_disabled_tests --gtest_repeat=10 --gtest_recreate_environments_when_repeating || exit 1
)

for /r "." %%a in (build\bin\*_stl.exe) do (
    echo -------------------------------------
    echo %%~na
    echo -------------------------------------
    %%~fa --gtest_also_run_disabled_tests --gtest_repeat=10 --gtest_recreate_environments_when_repeating || exit 1
)
