cmake_minimum_required(VERSION 3.16.3)
project(microlife-json)

# Set the compiler options
set(CMAKE_CXX_STANDARD 17)
if(MSVC)
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
else()
set(CMAKE_CXX_FLAGS "-g -O0 -Wall -coverage")
endif()

set(
  PROJECT_SOURCES
  "include/microlife/detail/value_t.h"
  "include/microlife/detail/basic_json.h"
  "include/microlife/detail/lexer.h"
  "include/microlife/json.h"

  "include/microlife/detail/macro_scope.h"
  "include/microlife/detail/macro_unscope.h"
  
  "microlife/detail/lexer.cpp"
  "microlife/detail/basic_json.cpp"
  "microlife/detail/parser.cpp"

  # unit test
  "test/unit_lexer.cpp"
  "test/unit_basic_json.cpp"
  "test/unit_parser.cpp"
  "test/unit_basic_json_dump.cpp"
  "test/unit_baisc_json_parser.cpp"

  "test/microlife_json.cpp"
)

include_directories("./include")

# gtest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://file.winterwonder.top/public/package/googletest-release-1.11.0.zip
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
