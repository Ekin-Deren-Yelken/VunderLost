cmake_minimum_required(VERSION 3.10)

project(VunderLost VERSION 0.1.0 LANGUAGES C CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Collect all source files in src/
file(GLOB_RECURSE SOURCES
    src/*.cpp
)

# Define the executable
add_executable(VunderLost ${SOURCES})

# Include header directories
target_include_directories(VunderLost PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
