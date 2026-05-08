add_library(hybrid_lib STATIC
    src/world.cpp
    src/hybrid_entity.cpp
    src/systems/ai_system.cpp
    src/systems/movement_system.cpp
    src/systems/interaction_system.cpp
)

target_include_directories(hybrid_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/third_party
)

target_link_libraries(hybrid_lib PUBLIC
    shared
    benchmark_lib
)
