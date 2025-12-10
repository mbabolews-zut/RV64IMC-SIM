cmake_minimum_required(VERSION 3.16)
project(RV64_SIM LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    add_definitions(-DDEBUG)
endif()

set(PARSER_SRC ${CMAKE_CURRENT_SOURCE_DIR}/source/parser/parser.yy)
set(LEXER_SRC  ${CMAKE_CURRENT_SOURCE_DIR}/source/parser/asm.l)
set(GEN_DIR    ${CMAKE_CURRENT_BINARY_DIR}/generated/parser)

file(MAKE_DIRECTORY ${GEN_DIR})

############################################
# WINDOWS / WSL PARSER GENERATION
############################################
if(WIN32)
    message(STATUS "Windows -> Using WSL for Bison/Flex")

    function(to_wsl_path WIN_PATH OUT_VAR)
        execute_process(
            COMMAND wsl wslpath -a "${WIN_PATH}"
            OUTPUT_VARIABLE _wsl
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        set(${OUT_VAR} "${_wsl}" PARENT_SCOPE)
    endfunction()

    to_wsl_path("${PARSER_SRC}" PARSER_SRC_WSL)
    to_wsl_path("${LEXER_SRC}"  LEXER_SRC_WSL)
    to_wsl_path("${GEN_DIR}"    GEN_DIR_WSL)

    add_custom_command(
        OUTPUT ${GEN_DIR}/parser.tab.cc ${GEN_DIR}/parser.tab.hh
        COMMAND wsl bison -d -o "${GEN_DIR_WSL}/parser.tab.cc" "${PARSER_SRC_WSL}"
        DEPENDS ${PARSER_SRC}
        COMMENT "WSL: Bison generating parser"
    )

    add_custom_command(
        OUTPUT ${GEN_DIR}/lex.yy.cc
        COMMAND wsl flex -+ -o "${GEN_DIR_WSL}/lex.yy.cc" "${LEXER_SRC_WSL}"
        DEPENDS ${LEXER_SRC}
        COMMENT "WSL: Flex generating lexer"
    )

    add_custom_command(
        OUTPUT ${GEN_DIR}/FlexLexer.h
        COMMAND wsl cp /usr/include/FlexLexer.h "${GEN_DIR_WSL}/FlexLexer.h"
        COMMENT "Copying FlexLexer.h from WSL"
    )

    add_definitions(-DYY_NO_UNISTD_H)
    

    add_custom_target(parser_gen ALL
        DEPENDS
            ${GEN_DIR}/parser.tab.cc
            ${GEN_DIR}/parser.tab.hh
            ${GEN_DIR}/lex.yy.cc
            ${GEN_DIR}/FlexLexer.h
    )


else()
    find_package(BISON REQUIRED)
    find_package(FLEX REQUIRED)

    BISON_TARGET(Parser
        ${PARSER_SRC}
        ${GEN_DIR}/parser.tab.cc
        DEFINES_FILE ${GEN_DIR}/parser.tab.hh
    )

    FLEX_TARGET(Scanner
        ${LEXER_SRC}
        ${GEN_DIR}/lex.yy.cc
    )

    ADD_FLEX_BISON_DEPENDENCY(Scanner Parser)

    add_custom_target(parser_gen ALL
        DEPENDS ${BISON_Parser_OUTPUTS} ${FLEX_Scanner_OUTPUTS}
    )
endif()

############################################
# INTERFACE LIBRARY EXPORTING PARSER FILES
############################################
add_library(parser_interface INTERFACE)
add_dependencies(parser_interface parser_gen)

target_include_directories(parser_interface INTERFACE ${GEN_DIR})
target_sources(parser_interface INTERFACE
    ${GEN_DIR}/parser.tab.cc
    ${GEN_DIR}/parser.tab.hh
    ${GEN_DIR}/lex.yy.cc
    ${GEN_DIR}/FlexLexer.h
)

############################################
# MAIN EXECUTABLE
############################################
file(GLOB_RECURSE PROJECT_SOURCES
    source/*.cpp source/*.hpp
)
list(FILTER PROJECT_SOURCES EXCLUDE REGEX ".*main\\.cpp$")

add_executable(RV64_SIM
    ${PROJECT_SOURCES}
    source/main.cpp
)

target_link_libraries(RV64_SIM PRIVATE parser_interface)
target_include_directories(RV64_SIM PRIVATE ${CMAKE_SOURCE_DIR}/source)

############################################
# OPTIONAL TESTS (independent)
############################################
option(BUILD_TESTS "Build tests" OFF)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
