add_library(ecs_entt_lib STATIC
    src/world.cpp
)

target_include_directories(ecs_entt_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/third_party
)

target_link_libraries(ecs_entt_lib PUBLIC shared)
