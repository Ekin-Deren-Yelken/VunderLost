cmake_minimum_required(VERSION 3.10.0)
project(VunderLost VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    main.cpp
    0-utils/rpg_utils.cpp
    1-Characters/character.cpp
    2-Enemies/enemies.cpp
    3-Story/story.cpp
    3-Story/act1.cpp
    3-Story/act2.cpp
    3-Story/act3.cpp
    3-Story/act4.cpp
    4-Saves/save_system.cpp
    5-Combat/CombatManager.cpp
)

add_executable(VunderLost ${SOURCES})

target_include_directories(VunderLost PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/0-utils
    ${CMAKE_CURRENT_SOURCE_DIR}/1-Characters
    ${CMAKE_CURRENT_SOURCE_DIR}/2-Enemies
    ${CMAKE_CURRENT_SOURCE_DIR}/3-Story
    ${CMAKE_CURRENT_SOURCE_DIR}/4-Saves
    ${CMAKE_CURRENT_SOURCE_DIR}/5-Combat
)