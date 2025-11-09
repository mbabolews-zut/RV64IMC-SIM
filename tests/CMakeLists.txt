find_package(Catch2 REQUIRED)

# Get parent's source files (exclude main.cpp)
file(GLOB_RECURSE PROJECT_SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/source/*.cpp
        ${CMAKE_SOURCE_DIR}/source/*.hpp
)
list(FILTER PROJECT_SOURCE_FILES EXCLUDE REGEX ".*main\\.cpp$")

# Auto-discover test files
file(GLOB TEST_FILES *.cpp)

add_executable(RV64_SIM_tests
        ${PROJECT_SOURCE_FILES}
        ${TEST_FILES}
        interp_instr_methods_test.cpp
)

target_include_directories(RV64_SIM_tests PRIVATE
        ${CMAKE_SOURCE_DIR}/source
        ${GEN_DIR}
)

target_link_libraries(RV64_SIM_tests PRIVATE Catch2::Catch2WithMain)

include(Catch)
catch_discover_tests(RV64_SIM_tests)