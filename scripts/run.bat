@echo off

cd build
ctest --extra-verbose --build-config Release --repeat-until-fail 10 --timeout 100000 --build-and-test
cd ..

for /r "." %%a in (build\bin\*_mpi.exe) do (
    echo -------------------------------------
    echo %%~na
    echo -------------------------------------
    for /l %%x in (1, 1, 1) do (
        "C:\Program Files\Microsoft MPI\Bin\mpiexec.exe" -np 4 %%~fa || exit 1
    )
)

@REM for /r "." %%a in (build\bin\*_ref.exe) do (
@REM     echo -------------------------------------
@REM     echo %%~na
@REM     echo -------------------------------------
@REM     %%~fa --gtest_repeat=10
@REM )
@REM
@REM for /r "." %%a in (build\bin\*_seq.exe) do (
@REM     echo -------------------------------------
@REM     echo %%~na
@REM     echo -------------------------------------
@REM     %%~fa --gtest_repeat=10
@REM )
@REM
@REM for /r "." %%a in (build\bin\*_omp.exe) do (
@REM     echo -------------------------------------
@REM     echo %%~na
@REM     echo -------------------------------------
@REM     %%~fa --gtest_repeat=10
@REM )
@REM
@REM for /r "." %%a in (build\bin\*_tbb.exe) do (
@REM     echo -------------------------------------
@REM     echo %%~na
@REM     echo -------------------------------------
@REM     %%~fa --gtest_repeat=10
@REM )
@REM
@REM for /r "." %%a in (build\bin\*_std.exe) do (
@REM     echo -------------------------------------
@REM     echo %%~na
@REM     echo -------------------------------------
@REM     %%~fa --gtest_repeat=10
@REM )
