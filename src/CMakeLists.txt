cmake_minimum_required(VERSION 3.10)
project(safetyboard_checks LANGUAGES C CXX)

# Set standards
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable testing
include(CTest)
enable_testing()

# Add the safetyboard library
add_library(safetyboard STATIC safety/safetyboard.c)
target_include_directories(safetyboard PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/safety)

# Add the test directory
if (BUILD_TESTS)
    add_subdirectory(safety/test)
endif()
