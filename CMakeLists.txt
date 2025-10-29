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
)

# ---- Main executable ----
add_executable(RV64_SIM ${SOURCE_FILES} source/main.cpp)
message(${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(RV64_SIM PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/source)

# ---- Testing ----
enable_testing()
add_subdirectory(tests)