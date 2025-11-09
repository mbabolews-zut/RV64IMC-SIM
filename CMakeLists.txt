cmake_minimum_required(VERSION 3.16)
project(RV64_SIM LANGUAGES CXX)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    add_definitions(-DDEBUG)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

set(PARSER_SRC ${CMAKE_CURRENT_SOURCE_DIR}/source/parser/parser.yy)
set(LEXER_SRC  ${CMAKE_CURRENT_SOURCE_DIR}/source/parser/asm.l)
set(GEN_DIR    ${CMAKE_CURRENT_BINARY_DIR}/source/parser)
file(MAKE_DIRECTORY ${GEN_DIR})

# Generate parser and lexer
BISON_TARGET(Parser
        ${PARSER_SRC}
        ${GEN_DIR}/parser.tab.cc
        DEFINES_FILE ${GEN_DIR}/parser.tab.hh
        COMPILE_FLAGS -d
)

FLEX_TARGET(Scanner
        ${LEXER_SRC}
        ${GEN_DIR}/lex.yy.cc
        COMPILE_FLAGS -+
)

ADD_FLEX_BISON_DEPENDENCY(Scanner Parser)

# Auto-discover source files
file(GLOB_RECURSE SOURCE_FILES
        source/*.cpp
        source/*.hpp
)

# Exclude main.cpp from source files (it's only for the executable)
list(FILTER SOURCE_FILES EXCLUDE REGEX ".*main\\.cpp$")

# Add generated parser/lexer files
list(APPEND SOURCE_FILES
        ${BISON_Parser_OUTPUTS}
        ${FLEX_Scanner_OUTPUTS}
        ${BISON_Parser_DEFINES_FILE}
)

# Main executable
add_executable(RV64_SIM ${SOURCE_FILES} source/main.cpp)

target_include_directories(RV64_SIM PRIVATE
        ${GEN_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)

# Optional testing (only if Catch2 is found)
find_package(Catch2 QUIET)
if(Catch2_FOUND)
    message(STATUS "Catch2 found - building tests")
    enable_testing()
    add_subdirectory(tests)
else()
    message(STATUS "Catch2 not found - skipping tests")
endif()
