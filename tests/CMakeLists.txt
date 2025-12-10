include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.0
)

FetchContent_MakeAvailable(Catch2)

file(GLOB TEST_FILES *.cpp)

add_executable(RV64_SIM_tests
    ${TEST_FILES}
)

target_link_libraries(RV64_SIM_tests PRIVATE
    parser_interface
    Catch2::Catch2WithMain
)

target_include_directories(RV64_SIM_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/source
    ${CMAKE_BINARY_DIR}/generated/parser
)

include(Catch)
catch_discover_tests(RV64_SIM_tests)
