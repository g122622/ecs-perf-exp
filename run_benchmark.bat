@echo off
setlocal EnableDelayedExpansion

echo ================================================================================
echo   ECS vs OOP vs EnTT ECS Performance Benchmark - Build and Run Script
echo ================================================================================
echo.

set SCRIPT_DIR=%~dp0
set BUILD_DIR=%SCRIPT_DIR%build
set RESULTS_DIR=%BUILD_DIR%\bin\Release\results

echo [1/4] Configuring CMake...
cmake -B "%BUILD_DIR%" -G "Visual Studio 17 2022" -A x64
if %ERRORLEVEL% neq 0 (
    echo CMake configuration failed!
    exit /b 1
)
echo.

echo [2/4] Building Release version...
cmake --build "%BUILD_DIR%" --config Release
if %ERRORLEVEL% neq 0 (
    echo Build failed!
    exit /b 1
)
echo.

echo [3/4] Running benchmarks...

REM Clear old results
del /Q "%RESULTS_DIR%\*.csv" 2>nul

echo.
echo   --- Running OOP Benchmark ---
"%BUILD_DIR%\bin\Release\oop_benchmark.exe"
if %ERRORLEVEL% neq 0 (
    echo OOP benchmark failed!
    exit /b 1
)

echo.
echo   --- Running EnTT ECS Benchmark ---
"%BUILD_DIR%\bin\Release\ecs_entt_benchmark.exe"
if %ERRORLEVEL% neq 0 (
    echo EnTT ECS benchmark failed!
    exit /b 1
)
echo.

echo [4/4] Generating visualization...
python "%SCRIPT_DIR%scripts\visualize.py" "%RESULTS_DIR%" "%RESULTS_DIR%"
if %ERRORLEVEL% neq 0 (
    echo Visualization failed!
    exit /b 1
)
echo.

echo ================================================================================
echo   Benchmark Complete!
echo ================================================================================
echo.
echo Results saved to: %RESULTS_DIR%
echo.
echo Generated files:
echo   - frame_time_comparison.png
echo   - avg_frame_time.png
echo   - performance_improvement.png
echo   - memory_comparison.png
echo   - creation_time.png
echo   - frame_time_distribution.png
echo   - benchmark_summary.txt
echo.

:: Open results folder
explorer "%RESULTS_DIR%"

endlocal
