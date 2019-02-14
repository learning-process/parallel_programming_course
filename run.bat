@echo off
for /r "." %%a in (build\bin\*_mpi.exe) do mpiexec -np 4 %%~fa
for /r "." %%a in (build\bin\*_omp.exe) do %%~fa
for /r "." %%a in (build\bin\*_tbb.exe) do %%~fa
