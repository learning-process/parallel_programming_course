@echo off
for /r "." %%a in (build\bin\*_mpi.exe) do (
    echo -------------------------------------
    echo %%~na
    echo -------------------------------------
    mpiexec -np 4 %%~fa
)

for /r "." %%a in (build\bin\*_omp.exe) do (
    echo -------------------------------------
    echo %%~na
    echo -------------------------------------
    %%~fa --gtest_repeat=10
)

for /r "." %%a in (build\bin\*_tbb.exe) do (
    echo -------------------------------------
    echo %%~na
    echo -------------------------------------
    %%~fa --gtest_repeat=10
)
