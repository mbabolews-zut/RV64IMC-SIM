cmake_minimum_required(VERSION 3.16)
project(RV64_SIM LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

set(PARSER_SRC ${CMAKE_CURRENT_SOURCE_DIR}/source/parser/parser.yy)
set(LEXER_SRC  ${CMAKE_CURRENT_SOURCE_DIR}/source/parser/asm.l)
set(GEN_DIR    ${CMAKE_CURRENT_BINARY_DIR}/source/parser)
file(MAKE_DIRECTORY ${GEN_DIR})

# Bison: generate parser.tab.cc and parser.tab.hh
BISON_TARGET(Parser
        ${PARSER_SRC}
        ${GEN_DIR}/parser.tab.cc
        DEFINES_FILE ${GEN_DIR}/parser.tab.hh
        COMPILE_FLAGS -d
)

# Flex: generate lex.yy.cc (C++ scanner)
FLEX_TARGET(Scanner
        ${LEXER_SRC}
        ${GEN_DIR}/lex.yy.cc
        COMPILE_FLAGS -+
)

ADD_FLEX_BISON_DEPENDENCY(Scanner Parser)

# ---- Sources (do NOT list generated parser files here) ----
set(SOURCE_FILES
        source/rv64/Cpu.cpp
        source/rv64/Cpu.hpp
        source/rv64/Interpreter.cpp
        source/rv64/Interpreter.hpp
        source/rv64/Memory.cpp
        source/rv64/Memory.hpp
        source/rv64/VM.cpp
        source/rv64/VM.hpp
        source/rv64/instruction_sets/IBase.hpp
        source/rv64/instruction_sets/IExtM.hpp
        source/Instruction.hpp
        source/common.hpp
        source/Instruction.cpp
        source/rv64/instruction_sets/Rv64IMC.hpp
        source/rv64/GPIntReg.cpp
        source/rv64/GPIntReg.hpp
        source/rv64/Reg.cpp
        source/rv64/Reg.hpp
        source/endianness.hpp
        source/rv64/PagedMemory.hpp
        source/rv64/PagedMemory.cpp
        source/parser/ParserProcessor.hpp
        source/parser/ParserProcessor.cpp
        # (omit source/parser/lex.yy.cc and parser.tab.* here)
)

# Append generated outputs from Bison/Flex into the sources
list(APPEND SOURCE_FILES
        ${BISON_Parser_OUTPUTS}       # parser.tab.cc
        ${FLEX_Scanner_OUTPUTS}       # lex.yy.cc
        ${BISON_Parser_DEFINES_FILE}  # parser.tab.hh (will be available to include)
)

# ---- Main executable (now the target exists) ----
add_executable(RV64_SIM ${SOURCE_FILES} source/main.cpp)

# Include generated headers and project sources
target_include_directories(RV64_SIM PRIVATE ${GEN_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/source)

# Ensure the executable depends on generated parser/lexer targets
#add_dependencies(RV64_SIM Parser Scanner)

# ---- Testing ----
enable_testing()
add_subdirectory(tests)