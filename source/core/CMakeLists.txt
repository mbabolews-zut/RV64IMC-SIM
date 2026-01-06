cmake_minimum_required(VERSION 3.16)
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

include(bisonflex.cmake)

############################################
# CORE LIBRARY
############################################
set(PROJECT_SOURCES
    BuildError.hpp
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
