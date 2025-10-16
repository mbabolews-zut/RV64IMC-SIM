cmake_minimum_required(VERSION 3.16)
project(RV64_SIM LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Sources ----
set(SOURCE_FILES
        source/rv64/Cpu.cpp
        source/rv64/Cpu.hpp
        source/rv64/Interpreter.cpp
        source/rv64/Interpreter.hpp
        source/rv64/Memory.cpp
        source/rv64/Memory.hpp
        source/rv64/VM.cpp
        source/rv64/VM.hpp
        source/rv64/InstructionSets/IBase.hpp
        source/rv64/InstructionSets/IExtM.hpp
        source/Instruction.hpp
        source/common.hpp
        source/LexicalAnalyzer.cpp
        source/LexicalAnalyzer.hpp
)

# ---- Main executable ----
add_executable(RV64_SIM ${SOURCE_FILES} source/main.cpp)

target_include_directories(RV64_SIM PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# ---- Testing ----
enable_testing()
add_subdirectory(tests)