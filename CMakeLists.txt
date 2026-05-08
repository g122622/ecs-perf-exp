cmake_minimum_required(VERSION 3.20)
project(ecs_perf_exp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# MSVC specific settings
if(MSVC)
    add_compile_options(/W4 /permissive- /Zc:__cplusplus /utf-8)
    # Optimization flags for Release
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /Ob3 /GL /arch:AVX2")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Add subdirectories
add_subdirectory(benchmark)
add_subdirectory(oop)
add_subdirectory(ecs_entt)
add_subdirectory(hybrid)

# Shared headers (header-only)
add_library(shared INTERFACE)
target_include_directories(shared INTERFACE ${CMAKE_SOURCE_DIR}/shared)

# OOP Benchmark executable
add_executable(oop_benchmark main_oop.cpp)
target_link_libraries(oop_benchmark PRIVATE benchmark_lib oop_lib shared)

# EnTT ECS Benchmark executable
add_executable(ecs_entt_benchmark main_ecs_entt.cpp)
target_link_libraries(ecs_entt_benchmark PRIVATE benchmark_lib ecs_entt_lib shared)

# Hybrid Benchmark executable
add_executable(hybrid_benchmark main_hybrid.cpp)
target_link_libraries(hybrid_benchmark PRIVATE benchmark_lib hybrid_lib shared)

