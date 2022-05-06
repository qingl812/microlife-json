cmake_minimum_required(VERSION 3.16.3)

project(microlife-json-test)

set(
	TEST_SOURCES
	# unit test
	"unit_lexer.cpp"
	"unit_basic_json.cpp"
	"unit_parser.cpp"
	"unit_basic_json_dump.cpp"
	"unit_baisc_json_parser.cpp"

	"microlife_json.cpp"

	"test.cpp"
)
include_directories(${CMAKE_SOURCE_DIR}/include)

# gtest
include(FetchContent)
FetchContent_Declare(
	googletest
	URL https://file.winterwonder.top/public/package/googletest-release-1.11.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(${PROJECT_NAME} ${TEST_SOURCES})
target_link_libraries(
	${PROJECT_NAME}
	microlife-json-lib
	gtest_main
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})
