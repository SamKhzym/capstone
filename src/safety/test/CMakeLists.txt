cmake_minimum_required(VERSION 3.10)
project(safetyboard_tests)

set(CMAKE_OSX_DEPLOYMENT_TARGET "14.0")

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

target_include_directories(safetyboard PUBLIC ../safety)
# include(FetchContent)
# FetchContent_Declare(
#   googletest
#   URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
# )
# # For Windows: Prevent overriding the parent project's compiler/linker settings
# set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# FetchContent_MakeAvailable(googletest)

find_package(GTest CONFIG REQUIRED)

enable_testing()

add_executable(
  test_crc
  test_crc.cc
)

target_link_libraries(
  test_crc
  PRIVATE
  safetyboard
  GTest::gtest
  GTest::gtest_main
  -stdlib=libc++
)

include(GoogleTest)
gtest_discover_tests(test_crc)