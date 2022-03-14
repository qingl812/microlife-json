cmake_minimum_required(VERSION 3.16.3)

project(microlife-json)

set(
  PROJECT_SOURCES
  "include/detail/basic_json.hpp"
  "include/detail/context_s.hpp"
  "include/detail/parse_t.hpp"
  "include/detail/parser.hpp"
  "include/detail/type_t.hpp"
  "include/detail/value_s.hpp"

  "test/unit_depend.hpp"
  "test/test_unit_depend.cc"
  "test/unit_paser.cc"
)
include_directories(include)

# gcov
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} --coverage")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")

# GoogleTest requires at least C++11
set(CMAKE_CXX_STANDARD 11)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

target_link_libraries(
  ${PROJECT_NAME}
  gtest_main
)


include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})

add_definitions(-Wall) # 允许发出 GCC 能够提供的所有有用的警告
