add_library(ecs_lib STATIC
    src/entity_manager.cpp
    src/component_pool.cpp
    src/world.cpp
    src/entity_factory.cpp
    src/systems/movement_system.cpp
    src/systems/physics_system.cpp
    src/systems/ai_system.cpp
    src/systems/decay_system.cpp
    src/systems/interaction_system.cpp
)

target_include_directories(ecs_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}
)
