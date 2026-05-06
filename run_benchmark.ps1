#!/usr/bin/env pwsh
# ECS vs OOP vs EnTT ECS Performance Benchmark - Build and Run Script

$ErrorActionPreference = "Stop"

Write-Host "================================================================================" -ForegroundColor Cyan
Write-Host "  ECS vs OOP vs EnTT ECS Performance Benchmark - Build and Run Script" -ForegroundColor Cyan
Write-Host "================================================================================" -ForegroundColor Cyan
Write-Host ""

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$BuildDir = Join-Path $ScriptDir "build"
$ResultsDir = Join-Path $BuildDir "bin\Release\results"

# Step 1: Configure CMake
Write-Host "[1/4] Configuring CMake..." -ForegroundColor Yellow
& cmake -B $BuildDir -G "Visual Studio 17 2022" -A x64
if ($LASTEXITCODE -ne 0) {
    Write-Host "CMake configuration failed!" -ForegroundColor Red
    exit 1
}
Write-Host ""

# Step 2: Build
Write-Host "[2/4] Building Release version..." -ForegroundColor Yellow
& cmake --build $BuildDir --config Release
if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed!" -ForegroundColor Red
    exit 1
}
Write-Host ""

# Step 3: Run benchmarks
Write-Host "[3/4] Running benchmarks..." -ForegroundColor Yellow

Write-Host ""
Write-Host "  --- Running OOP Benchmark ---" -ForegroundColor White
& "$BuildDir\bin\Release\oop_benchmark.exe"
if ($LASTEXITCODE -ne 0) {
    Write-Host "OOP benchmark failed!" -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "  --- Running ECS Benchmark ---" -ForegroundColor White
& "$BuildDir\bin\Release\ecs_benchmark.exe"
if ($LASTEXITCODE -ne 0) {
    Write-Host "ECS benchmark failed!" -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "  --- Running EnTT ECS Benchmark ---" -ForegroundColor White
& "$BuildDir\bin\Release\ecs_entt_benchmark.exe"
if ($LASTEXITCODE -ne 0) {
    Write-Host "EnTT ECS benchmark failed!" -ForegroundColor Red
    exit 1
}
Write-Host ""

# Step 4: Visualization
Write-Host "[4/4] Generating visualization..." -ForegroundColor Yellow
& python "$ScriptDir\scripts\visualize.py" $ResultsDir $ResultsDir
if ($LASTEXITCODE -ne 0) {
    Write-Host "Visualization failed!" -ForegroundColor Red
    exit 1
}
Write-Host ""

Write-Host "================================================================================" -ForegroundColor Green
Write-Host "  Benchmark Complete!" -ForegroundColor Green
Write-Host "================================================================================" -ForegroundColor Green
Write-Host ""
Write-Host "Results saved to: $ResultsDir" -ForegroundColor White
Write-Host ""
Write-Host "Generated files:" -ForegroundColor White
Write-Host "  - frame_time_comparison.png"
Write-Host "  - avg_frame_time.png"
Write-Host "  - performance_improvement.png"
Write-Host "  - memory_comparison.png"
Write-Host "  - creation_time.png"
Write-Host "  - frame_time_distribution.png"
Write-Host "  - benchmark_summary.txt"
Write-Host ""

# Open results folder
Start-Process "explorer.exe" $ResultsDir
