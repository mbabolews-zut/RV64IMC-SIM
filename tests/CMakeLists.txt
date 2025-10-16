find_package(Catch2 REQUIRED)

# Reuse source files from the main project
set(TEST_SOURCES
        ${CMAKE_SOURCE_DIR}/source/common.hpp
        ${CMAKE_SOURCE_DIR}/source/rv64/Cpu.cpp
        ${CMAKE_SOURCE_DIR}/source/rv64/Interpreter.cpp
        ${CMAKE_SOURCE_DIR}/source/rv64/Memory.cpp
        ${CMAKE_SOURCE_DIR}/source/rv64/VM.cpp
        ../source/LexicalAnalyzer.cpp
        ../source/LexicalAnalyzer.hpp
        # Add others if needed
)

add_executable(RV64_SIM_tests
        ${TEST_SOURCES}
        memory_test.cpp
)

target_include_directories(RV64_SIM_tests PRIVATE
        ${CMAKE_SOURCE_DIR}/source
)

target_link_libraries(RV64_SIM_tests PRIVATE Catch2::Catch2WithMain)

include(Catch)
catch_discover_tests(RV64_SIM_tests)