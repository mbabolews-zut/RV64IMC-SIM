cmake_minimum_required(VERSION 3.31)
project(RV64_SIM)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)

add_executable(RV64_SIM main.cpp
        rv64/Cpu.cpp
        rv64/Cpu.hpp
        common.hpp
        rv64/InstructionSets/IBase.hpp
        rv64/InstructionSets/IExtM.hpp
        rv64/InterpreterRV64IM.cpp
        rv64/InterpreterRV64IM.hpp
        rv64/Memory.cpp
        rv64/Memory.hpp
        rv64/VM.cpp
        rv64/VM.hpp)
