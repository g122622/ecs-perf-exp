add_library(oop_lib STATIC
    src/entity.cpp
    src/living_entity.cpp
    src/creature.cpp
    src/monster.cpp
    src/animal.cpp
    src/flying_creature.cpp
    src/aquatic_creature.cpp
    src/world.cpp
    src/entities/monsters.cpp
    src/entities/animals.cpp
    src/entities/flying.cpp
    src/entities/aquatic.cpp
)

target_include_directories(oop_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}
)
