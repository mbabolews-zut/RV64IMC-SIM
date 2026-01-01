cmake_minimum_required(VERSION 3.16)
cmake_policy(SET CMP0100 NEW)
project(RV64_SIM_CORE LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    add_definitions(-DDEBUG)
endif()

############################################
# PARSER CONFIGURATION OPTIONS
############################################
# PREGENERATED_PARSER_DIR: Path to pre-generated parser files
# If set, uses files from this directory instead of running bison/flex
# Example: cmake -DPREGENERATED_PARSER_DIR=/path/to/parser/generated ..
set(PREGENERATED_PARSER_DIR "" CACHE PATH
    "Path to pre-generated parser files (parser.tab.cc, parser.tab.hh, lex.yy.cc). Leave empty to generate with bison/flex.")

set(PARSER_SRC ${CMAKE_CURRENT_SOURCE_DIR}/parser/parser.yy)
set(LEXER_SRC  ${CMAKE_CURRENT_SOURCE_DIR}/parser/asm.l)
set(GEN_DIR    ${CMAKE_CURRENT_BINARY_DIR}/generated/parser)

# Default location for saving/loading pre-generated files
set(DEFAULT_PREGEN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/parser/generated)

file(MAKE_DIRECTORY ${GEN_DIR})

############################################
# PARSER GENERATION LOGIC
############################################
if(PREGENERATED_PARSER_DIR)
    # Use pre-generated parser files from specified directory
    set(PREGEN_DIR "${PREGENERATED_PARSER_DIR}")
    message(STATUS "Using pre-generated parser files from: ${PREGEN_DIR}")

    # Validate required files exist
    set(_missing_files "")
    foreach(_file parser.tab.cc parser.tab.hh lex.yy.cc)
        if(NOT EXISTS "${PREGEN_DIR}/${_file}")
            list(APPEND _missing_files "${_file}")
        endif()
    endforeach()

    if(_missing_files)
        message(FATAL_ERROR
            "Pre-generated parser files missing in ${PREGEN_DIR}:\n"
            "  ${_missing_files}\n"
            "Required files: parser.tab.cc, parser.tab.hh, lex.yy.cc\n"
            "Either provide correct path or remove PREGENERATED_PARSER_DIR to generate with bison/flex"
        )
    endif()

    # Copy pre-generated files to build directory
    configure_file(${PREGEN_DIR}/parser.tab.cc ${GEN_DIR}/parser.tab.cc COPYONLY)
    configure_file(${PREGEN_DIR}/parser.tab.hh ${GEN_DIR}/parser.tab.hh COPYONLY)
    configure_file(${PREGEN_DIR}/lex.yy.cc ${GEN_DIR}/lex.yy.cc COPYONLY)

    if(EXISTS "${PREGEN_DIR}/FlexLexer.h")
        configure_file(${PREGEN_DIR}/FlexLexer.h ${GEN_DIR}/FlexLexer.h COPYONLY)
    endif()

    add_custom_target(parser_gen ALL
        COMMENT "Using pre-generated parser files from ${PREGEN_DIR}"
    )

elseif(WIN32)
    ############################################
    # WINDOWS: Check PATH first, then WSL
    ############################################
    find_program(BISON_EXECUTABLE bison)
    find_program(FLEX_EXECUTABLE flex)

    if(BISON_EXECUTABLE AND FLEX_EXECUTABLE)
        # Native Windows bison/flex found in PATH
        message(STATUS "Windows: Found native Bison at ${BISON_EXECUTABLE}")
        message(STATUS "Windows: Found native Flex at ${FLEX_EXECUTABLE}")

        add_custom_command(
            OUTPUT ${GEN_DIR}/parser.tab.cc ${GEN_DIR}/parser.tab.hh
            COMMAND ${BISON_EXECUTABLE} -d -o "${GEN_DIR}/parser.tab.cc" "${PARSER_SRC}"
            DEPENDS ${PARSER_SRC}
            COMMENT "Bison: Generating parser"
            VERBATIM
        )

        add_custom_command(
            OUTPUT ${GEN_DIR}/lex.yy.cc
            COMMAND ${FLEX_EXECUTABLE} -+ -o "${GEN_DIR}/lex.yy.cc" "${LEXER_SRC}"
            DEPENDS ${LEXER_SRC}
            COMMENT "Flex: Generating lexer"
            VERBATIM
        )

        # Try to find FlexLexer.h
        find_path(FLEXLEXER_INCLUDE_DIR FlexLexer.h
            HINTS
                "${FLEX_EXECUTABLE}/../include"
                "${FLEX_EXECUTABLE}/../../include"
                "C:/msys64/usr/include"
                "C:/cygwin64/usr/include"
        )

        if(FLEXLEXER_INCLUDE_DIR)
            configure_file(${FLEXLEXER_INCLUDE_DIR}/FlexLexer.h ${GEN_DIR}/FlexLexer.h COPYONLY)
        else()
            message(WARNING "FlexLexer.h not found - you may need to copy it manually to ${GEN_DIR}")
        endif()

        add_definitions(-DYY_NO_UNISTD_H)

        add_custom_target(parser_gen ALL
            DEPENDS
                ${GEN_DIR}/parser.tab.cc
                ${GEN_DIR}/parser.tab.hh
                ${GEN_DIR}/lex.yy.cc
        )

    else()
        # Fall back to WSL
        message(STATUS "Windows: Native bison/flex not found, trying WSL...")

        find_program(WSL_EXECUTABLE wsl)
        if(NOT WSL_EXECUTABLE)
            message(FATAL_ERROR
                "Neither native bison/flex nor WSL found.\n"
                "Options:\n"
                "  1. Install bison/flex and add to PATH (e.g., via MSYS2, Cygwin, or winflexbison)\n"
                "  2. Install WSL with bison/flex: wsl --install && wsl sudo apt install bison flex\n"
                "  3. Use pre-generated files: cmake -DPREGENERATED_PARSER_DIR=/path/to/generated .."
            )
        endif()

        message(STATUS "Windows: Using WSL for Bison/Flex")

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
    endif()

else()
    ############################################
    # UNIX/LINUX: Use system bison/flex
    ############################################
    find_package(BISON REQUIRED)
    find_package(FLEX REQUIRED)

    message(STATUS "Found Bison: ${BISON_EXECUTABLE}")
    message(STATUS "Found Flex: ${FLEX_EXECUTABLE}")

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
# TARGET: Save generated parser to source tree
############################################
add_custom_target(save_generated_parser
    COMMAND ${CMAKE_COMMAND} -E make_directory "${DEFAULT_PREGEN_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${GEN_DIR}/parser.tab.cc" "${DEFAULT_PREGEN_DIR}/"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${GEN_DIR}/parser.tab.hh" "${DEFAULT_PREGEN_DIR}/"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${GEN_DIR}/lex.yy.cc" "${DEFAULT_PREGEN_DIR}/"
    COMMAND ${CMAKE_COMMAND} -E echo "Parser files saved to ${DEFAULT_PREGEN_DIR}"
    COMMAND ${CMAKE_COMMAND} -E echo "Use with: cmake -DPREGENERATED_PARSER_DIR=${DEFAULT_PREGEN_DIR} .."
    DEPENDS parser_gen
    COMMENT "Saving generated parser files to source tree for distribution"
)

if(WIN32)
    add_custom_command(TARGET save_generated_parser POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${GEN_DIR}/FlexLexer.h" "${DEFAULT_PREGEN_DIR}/" || true
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
)

if(WIN32)
    target_sources(parser_interface INTERFACE ${GEN_DIR}/FlexLexer.h)
endif()

############################################
# CORE LIBRARY
############################################
set(PROJECT_SOURCES
    common.hpp
    endianness.hpp
    Instruction.cpp
    Instruction.hpp
    InstructionBuilder.cpp
    InstructionBuilder.hpp
    intN.hpp
    Memory.cpp
    Memory.hpp
    PagedMemory.cpp
    PagedMemory.hpp
    ui.cpp
    ui.hpp
    rv64/AssemblerUnit.cpp
    rv64/AssemblerUnit.hpp
    rv64/Cpu.cpp
    rv64/Cpu.hpp
    rv64/Reg.cpp
    rv64/Reg.hpp
    rv64/GPIntReg.hpp
    rv64/GPIntReg.cpp
    rv64/Interpreter.cpp
    rv64/Interpreter.hpp
    rv64/VM.hpp
    rv64/VM.cpp
    rv64/instruction_sets/IBaseI.hpp
    rv64/instruction_sets/IExtensionM.hpp
    rv64/instruction_sets/IExtensionC.hpp
    rv64/instruction_sets/Rv64IMC.hpp
)

file(GLOB PARSER_IMPL ${CMAKE_CURRENT_SOURCE_DIR}/parser/*.cpp)
list(APPEND PROJECT_SOURCES ${PARSER_IMPL})

add_library(RV64_SIM_CORE STATIC ${PROJECT_SOURCES})

# Ensure generated parser files are available before building
add_dependencies(RV64_SIM_CORE parser_gen)

# Compile generated parser sources into core library
target_sources(RV64_SIM_CORE PRIVATE
    ${GEN_DIR}/parser.tab.cc
    ${GEN_DIR}/lex.yy.cc
)

target_link_libraries(RV64_SIM_CORE PUBLIC parser_interface)
target_include_directories(RV64_SIM_CORE PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
