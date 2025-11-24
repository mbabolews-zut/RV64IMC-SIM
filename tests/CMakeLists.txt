find_package(Catch2 REQUIRED)

# Get parent's source files (exclude main.cpp)
file(GLOB_RECURSE PROJECT_SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/source/*.cpp
        ${CMAKE_SOURCE_DIR}/source/*.hpp
)
list(FILTER PROJECT_SOURCE_FILES EXCLUDE REGEX ".*main\\.cpp$")

# Auto-discover test files
file(GLOB TEST_FILES *.cpp)

# Generated parser/lexer outputs from top-level build directory
set(GEN_PARSER_CC ${CMAKE_BINARY_DIR}/source/parser/parser.tab.cc)
set(GEN_LEXER_CC  ${CMAKE_BINARY_DIR}/source/parser/lex.yy.cc)
set(GEN_PARSER_HH ${CMAKE_BINARY_DIR}/source/parser/parser.tab.hh)

# Tell CMake these will be generated at build time (so configure won't fail)
set_source_files_properties(${GEN_PARSER_CC} ${GEN_LEXER_CC} ${GEN_PARSER_HH} PROPERTIES GENERATED TRUE)

add_executable(RV64_SIM_tests
        ${PROJECT_SOURCE_FILES}
        ${TEST_FILES}
        ${GEN_PARSER_CC}
        ${GEN_LEXER_CC}
        interp_instr_methods_test.cpp
)

target_include_directories(RV64_SIM_tests PRIVATE
        ${CMAKE_SOURCE_DIR}/source
        ${CMAKE_BINARY_DIR}/source/parser
)

# Ensure parser generation runs before test build if the top-level defined BISON/FLEX targets exist
if(TARGET Parser)
    add_dependencies(RV64_SIM_tests Parser)
endif()
if(TARGET Scanner)
    add_dependencies(RV64_SIM_tests Scanner)
endif()

target_link_libraries(RV64_SIM_tests PRIVATE Catch2::Catch2WithMain)

include(Catch)
catch_discover_tests(RV64_SIM_tests)