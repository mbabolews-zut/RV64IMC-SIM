cmake_minimum_required(VERSION 3.21)

project(RV64_SIM_TESTS LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

enable_testing()

include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.0
)

FetchContent_MakeAvailable(Catch2)

file(GLOB TEST_FILES
    ${CMAKE_SOURCE_DIR}/tests/rv64c_test.cpp
    ${CMAKE_SOURCE_DIR}/tests/rv64i_test.cpp
    ${CMAKE_SOURCE_DIR}/tests/rv64m_test.cpp
    ${CMAKE_SOURCE_DIR}/tests/memory_test.cpp
)

list(FILTER PROJECT_SOURCES EXCLUDE REGEX ".*main\\.cpp$")

add_executable(RV64_SIM_TESTS
    ${TEST_FILES}
)
add_definitions(-DTESTING=1)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../source/core ${CMAKE_BINARY_DIR}/core)
target_link_libraries(RV64_SIM_TESTS PRIVATE RV64_SIM_CORE)


target_link_libraries(RV64_SIM_TESTS PRIVATE
    parser_interface
    Catch2::Catch2WithMain
)

target_include_directories(RV64_SIM_TESTS PRIVATE
    ${CMAKE_SOURCE_DIR}/source/core
    ${CMAKE_BINARY_DIR}/generated/parser
)

include(Catch)
catch_discover_tests(RV64_SIM_TESTS)
