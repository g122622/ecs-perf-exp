add_library(benchmark_lib STATIC
    src/timer.cpp
    src/memory_tracker.cpp
    src/hw_counter.cpp
    src/benchmark_runner.cpp
)

target_include_directories(benchmark_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}
)
