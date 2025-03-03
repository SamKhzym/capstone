cmake_minimum_required(VERSION 3.10)

# == project/user settings == #
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(VCPKG_DIR $ENV{VCPKG_DIR})
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_ELF ON CACHE BOOL "Build Zephyr ELF")
set(BUILD_TESTS OFF CACHE BOOL "Build Tests")

set(PROJECT_BASE "${CMAKE_CURRENT_LIST_DIR}/../..")
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BASE}/bin)
# ================== #

# glob to find all files that need to be included
file(GLOB_RECURSE acc_sources ${PROJECT_BASE}/src/**.c)
file(GLOB_RECURSE arduino_sources ${PROJECT_BASE}/../arduino/**.c)

set(BOARD stm32h735g_disco)
set(includes ${PROJECT_BASE}/include ${PROJECT_BASE}/../arduino)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(safety_board)

target_include_directories(app PUBLIC ${includes})
target_sources(app PRIVATE ${acc_sources} ${arduino_sources})

include(${ZEPHYR_BASE}/samples/net/common/common.cmake)
