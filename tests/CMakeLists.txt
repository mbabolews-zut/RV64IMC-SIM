cmake_minimum_required(VERSION 3.21)

# Standalone build support
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(RV64_SIM_TESTS LANGUAGES CXX)
    project(RV64_SIM_PERF_TEST LANGUAGES CXX)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    enable_testing()
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../source/core ${CMAKE_BINARY_DIR}/core)
endif ()

############################################
# FETCH CATCH2
############################################
include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.12.0
)

FetchContent_MakeAvailable(Catch2)

############################################
# TEST EXECUTABLE
############################################
set(TEST_SOURCES
        error_handling_test.cpp
        integration_test.cpp
        parser_test.cpp
        rv64c_test.cpp
        rv64i_test.cpp
        rv64m_test.cpp
        memory_test.cpp
)

# Only include toolchain tests on Unix (requires popen/pclose and GNU toolchain)
if (UNIX)
    list(APPEND TEST_SOURCES assembler_test.cpp)
endif ()

add_executable(rv64_tests ${TEST_SOURCES})
add_executable(RV64_SIM_PERF_TEST performance_test.cpp)

target_compile_definitions(rv64_tests PRIVATE TESTING=1)

target_link_libraries(rv64_tests PRIVATE
        RV64_SIM_CORE
        Catch2::Catch2WithMain
)
target_link_libraries(RV64_SIM_PERF_TEST PRIVATE
        RV64_SIM_CORE
)

target_include_directories(RV64_SIM_PERF_TEST PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../source/core
)


target_include_directories(rv64_tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../source/core
)

############################################
# REGISTER TESTS WITH CTEST
############################################
include(Catch)
catch_discover_tests(rv64_tests)
