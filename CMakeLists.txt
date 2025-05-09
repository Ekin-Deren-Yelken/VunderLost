cmake_minimum_required(VERSION 3.10)

project(VunderLost VERSION 0.1.0 LANGUAGES C CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find dependencies
find_package(CURL REQUIRED)

# Source files
set(SOURCES
    main.cpp
    src/Characters/character.cpp
    src/Characters/npc_factory.cpp
    src/Combat/Ability.cpp
    src/Combat/AbilityLoader.cpp
    src/Combat/CombatManager.cpp
    src/Combat/Effect.cpp
    src/Combat/Status.cpp
    src/story/act1.cpp
    src/story/story.cpp
    src/Utilities/apikey_loader.cpp
    src/Utilities/chatgpt_wrapper.cpp
    src/Utilities/rpg_utils.cpp
    src/Utilities/save_system.cpp
    src/Utilities/trade.cpp
    src/enemies.cpp
)

# Executable
add_executable(VunderLost ${SOURCES})

# Include directories
target_include_directories(VunderLost PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/0-utils
    ${CMAKE_CURRENT_SOURCE_DIR}/1-Characters
    ${CMAKE_CURRENT_SOURCE_DIR}/3-Story
    ${CMAKE_CURRENT_SOURCE_DIR}/4-Saves
    ${CMAKE_CURRENT_SOURCE_DIR}/5-Combat
)

# Link libraries
target_link_libraries(VunderLost PRIVATE CURL::libcurl)
