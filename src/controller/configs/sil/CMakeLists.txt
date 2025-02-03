cmake_minimum_required(VERSION 3.10)
project(controller)

# == project/user settings == #
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(VCPKG_DIR $ENV{VCPKG_DIR})
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_TESTS OFF CACHE BOOL "Build Tests")

set(PROJECT_BASE "${CMAKE_CURRENT_LIST_DIR}/../..")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BASE}/bin)

add_compile_definitions(EXPORT_DLL) # build to dll target
# ================== #

message(${PROJECT_BASE})

# glob to find all files that need to be included
set(acc_sources ${PROJECT_BASE}/src/acc.c)

# Target definitions
add_library(${PROJECT_NAME}_static STATIC ${acc_sources})
add_library(${PROJECT_NAME}_shared SHARED ${acc_sources})
if (MSVC)
    add_compile_options(/O2)
else()
    add_compile_options(-O3)
    #target_compile_options(${PROJECT_NAME} PUBLIC -O0 -g) # uncomment for debugging purposes
endif()

# Include directories.
include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_BASE}/include)

# Linking phase
link_libraries(${PROJECT_NAME} PRIVATE)

# == GOOGLETEST BUILD == #
if(BUILD_TESTS) 
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(CMAKE_GTEST_DISCOVER_TESTS_DISCOVERY_MODE PRE_TEST)
    SET(GCC_COVERAGE_COMPILE_FLAGS "-fprofile-arcs -ftest-coverage -fno-inline")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}") 
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")
    enable_testing()
    find_package(GTest CONFIG REQUIRED)
    file(GLOB_RECURSE tests test/*.cpp)
    add_executable(test_${PROJECT_NAME} ${tests})
    target_compile_options(test_${PROJECT_NAME} PUBLIC)
    target_include_directories(test_${PROJECT_NAME} PUBLIC 
        include
        src 
        test)
    target_link_libraries(test_${PROJECT_NAME} PRIVATE  
        ${PROJECT_NAME} # Linking the controls library.
        GTest::gtest
        GTest::gtest_main
    )
    include(GoogleTest)
    gtest_discover_tests(test_${PROJECT_NAME})
endif()
# ====================== #
