@echo off

for /r "." %%a in (build\bin\*_seq.exe) do (
    echo -------------------------------------
    echo %%~na
    echo -------------------------------------
    %%~fa --gtest_repeat=10
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

for /r "." %%a in (build\bin\*_std.exe) do (
    echo -------------------------------------
    echo %%~na
    echo -------------------------------------
    %%~fa --gtest_repeat=10
)

for /r "." %%a in (build\bin\*_mpi.exe) do (
    echo -------------------------------------
    echo %%~na
    echo -------------------------------------
    for /l %%x in (1, 1, 1) do (
        "C:\Program Files\Microsoft MPI\Bin\mpiexec.exe" -np 4 %%~fa || exit 1
    )
)
