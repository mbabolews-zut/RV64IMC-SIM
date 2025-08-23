#pragma once
#include <rv64/Cpu.hpp>
#include <rv64/Memory.hpp>

namespace rv64 {
    class VM {
    public:
        void terminate(int exit_code);

        void error_stop();

        void breakpoint_hit();

    public:
        Memory m_memory;
        Cpu m_cpu;
    };
} // rv64