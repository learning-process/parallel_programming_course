@echo off
Setlocal EnableDelayedExpansion

for /r "." %%a in (build\bin\*_seq.exe) do (
    echo -------------------------------------
    echo %%~na
    echo -------------------------------------
    set start=!time!
    %%~fa --gtest_repeat=10
    set end=!time!

    set options="tokens=1-4 delims=:.,"
    for /f %options% %%a in ("!start!") do (
        set start_h=%%a
        set /a start_m=100%%b %% 100
        set /a start_s=100%%c %% 100
        set /a start_ms=100%%d %% 100
    )
    for /f %options% %%a in ("!end!") do (
        set end_h=%%a
        set /a end_m=100%%b %% 100
        set /a end_s=100%%c %% 100
        set /a end_ms=100%%d %% 100
    )

    set /a hours=!end_h!-!start_h!
    set /a mins=!end_m!-!start_m!
    set /a secs=!end_s!-!start_s!
    set /a ms=!end_ms!-!start_ms!

    if !ms! lss 0 set /a secs = !secs! - 1 & set /a ms = 100!ms!
    if !secs! lss 0 set /a mins = !mins! - 1 & set /a secs = 60!secs!
    if !mins! lss 0 set /a hours = !hours! - 1 & set /a mins = 60!mins!
    if !hours! lss 0 set /a hours = 24!hours!
    if 1!ms! lss 100 set ms=0!ms!
    set /a totalsecs = !hours!*3600 + !mins!*60 + !secs!
    if !totalsecs! gtr 5 (
        echo Alert: runtime greater then 5 sec.
        echo runtime = !totalsecs! sec.
        exit /b 1
    )
)

for /r "." %%a in (build\bin\*_omp.exe) do (
    echo -------------------------------------
    echo %%~na
    echo -------------------------------------
    set start=!time!
    %%~fa --gtest_repeat=10
    set end=!time!

    set options="tokens=1-4 delims=:.,"
    for /f %options% %%a in ("!start!") do (
        set start_h=%%a
        set /a start_m=100%%b %% 100
        set /a start_s=100%%c %% 100
        set /a start_ms=100%%d %% 100
    )
    for /f %options% %%a in ("!end!") do (
        set end_h=%%a
        set /a end_m=100%%b %% 100
        set /a end_s=100%%c %% 100
        set /a end_ms=100%%d %% 100
    )

    set /a hours=!end_h!-!start_h!
    set /a mins=!end_m!-!start_m!
    set /a secs=!end_s!-!start_s!
    set /a ms=!end_ms!-!start_ms!

    if !ms! lss 0 set /a secs = !secs! - 1 & set /a ms = 100!ms!
    if !secs! lss 0 set /a mins = !mins! - 1 & set /a secs = 60!secs!
    if !mins! lss 0 set /a hours = !hours! - 1 & set /a mins = 60!mins!
    if !hours! lss 0 set /a hours = 24!hours!
    if 1!ms! lss 100 set ms=0!ms!
    set /a totalsecs = !hours!*3600 + !mins!*60 + !secs!
    if !totalsecs! gtr 5 (
        echo Alert: runtime greater then 5 sec.
        echo runtime = !totalsecs! sec.
        exit /b 1
    )
)

for /r "." %%a in (build\bin\*_tbb.exe) do (
    echo -------------------------------------
    echo %%~na
    echo -------------------------------------
    set start=!time!
    %%~fa --gtest_repeat=10
    set end=!time!

    set options="tokens=1-4 delims=:.,"
    for /f %options% %%a in ("!start!") do (
        set start_h=%%a
        set /a start_m=100%%b %% 100
        set /a start_s=100%%c %% 100
        set /a start_ms=100%%d %% 100
    )
    for /f %options% %%a in ("!end!") do (
        set end_h=%%a
        set /a end_m=100%%b %% 100
        set /a end_s=100%%c %% 100
        set /a end_ms=100%%d %% 100
    )

    set /a hours=!end_h!-!start_h!
    set /a mins=!end_m!-!start_m!
    set /a secs=!end_s!-!start_s!
    set /a ms=!end_ms!-!start_ms!

    if !ms! lss 0 set /a secs = !secs! - 1 & set /a ms = 100!ms!
    if !secs! lss 0 set /a mins = !mins! - 1 & set /a secs = 60!secs!
    if !mins! lss 0 set /a hours = !hours! - 1 & set /a mins = 60!mins!
    if !hours! lss 0 set /a hours = 24!hours!
    if 1!ms! lss 100 set ms=0!ms!
    set /a totalsecs = !hours!*3600 + !mins!*60 + !secs!
    if !totalsecs! gtr 5 (
        echo Alert: runtime greater then 5 sec.
        echo runtime = !totalsecs! sec.
        exit /b 1
    )
)

for /r "." %%a in (build\bin\*_std.exe) do (
    echo -------------------------------------
    echo %%~na
    echo -------------------------------------
    set start=!time!
    %%~fa --gtest_repeat=10
    set end=!time!

    set options="tokens=1-4 delims=:.,"
    for /f %options% %%a in ("!start!") do (
        set start_h=%%a
        set /a start_m=100%%b %% 100
        set /a start_s=100%%c %% 100
        set /a start_ms=100%%d %% 100
    )
    for /f %options% %%a in ("!end!") do (
        set end_h=%%a
        set /a end_m=100%%b %% 100
        set /a end_s=100%%c %% 100
        set /a end_ms=100%%d %% 100
    )

    set /a hours=!end_h!-!start_h!
    set /a mins=!end_m!-!start_m!
    set /a secs=!end_s!-!start_s!
    set /a ms=!end_ms!-!start_ms!

    if !ms! lss 0 set /a secs = !secs! - 1 & set /a ms = 100!ms!
    if !secs! lss 0 set /a mins = !mins! - 1 & set /a secs = 60!secs!
    if !mins! lss 0 set /a hours = !hours! - 1 & set /a mins = 60!mins!
    if !hours! lss 0 set /a hours = 24!hours!
    if 1!ms! lss 100 set ms=0!ms!
    set /a totalsecs = !hours!*3600 + !mins!*60 + !secs!
    if !totalsecs! gtr 5 (
        echo Alert: runtime greater then 5 sec.
        echo runtime = !totalsecs! sec.
        exit /b 1
    )
)

for /r "." %%a in (build\bin\*_mpi.exe) do (
    echo -------------------------------------
    echo %%~na
    echo -------------------------------------
    set start=!time!
    for /l %%x in (1, 1, 10) do (
        mpiexec -np 4 %%~fa || exit /b 1
    )
    set end=!time!

    set options="tokens=1-4 delims=:.,"
    for /f %options% %%a in ("!start!") do (
        set start_h=%%a
        set /a start_m=100%%b %% 100
        set /a start_s=100%%c %% 100
        set /a start_ms=100%%d %% 100
    )
    for /f %options% %%a in ("!end!") do (
        set end_h=%%a
        set /a end_m=100%%b %% 100
        set /a end_s=100%%c %% 100
        set /a end_ms=100%%d %% 100
    )

    set /a hours=!end_h!-!start_h!
    set /a mins=!end_m!-!start_m!
    set /a secs=!end_s!-!start_s!
    set /a ms=!end_ms!-!start_ms!

    if !ms! lss 0 set /a secs = !secs! - 1 & set /a ms = 100!ms!
    if !secs! lss 0 set /a mins = !mins! - 1 & set /a secs = 60!secs!
    if !mins! lss 0 set /a hours = !hours! - 1 & set /a mins = 60!mins!
    if !hours! lss 0 set /a hours = 24!hours!
    if 1!ms! lss 100 set ms=0!ms!
    set /a totalsecs = !hours!*3600 + !mins!*60 + !secs!
    if !totalsecs! gtr 5 (
        echo Alert: runtime greater then 5 sec.
        echo runtime = !totalsecs! sec.
        exit /b 1
    )
)
