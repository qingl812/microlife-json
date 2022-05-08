cmake_minimum_required(VERSION 3.16.3)
project(microlife-json)

# Set the compiler options
set(CMAKE_CXX_STANDARD 17)
# msvc or gcc
if(MSVC)
add_compile_options("/utf-8")
else()
set(CMAKE_CXX_FLAGS "-g -O0 -Wall -coverage")
endif()

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

set(
	PROJECT_SOURCES
	# header files
	"include/microlife/detail/value_t.hpp"
	"include/microlife/detail/token_t.hpp"
	"include/microlife/detail/lexer.hpp"
	"include/microlife/detail/parser.hpp"
	"include/microlife/detail/macro_scope.hpp"
	"include/microlife/detail/macro_unscope.hpp"
	"include/microlife/detail/basic_json.hpp"

	"include/microlife/json.hpp"
)
include_directories("include")

add_executable(
	microlife-json
	${PROJECT_SOURCES}

	"example/example.cpp"
)

enable_testing()
add_subdirectory(test)
