#pragma once
#include <rv64/Cpu.hpp>
#include <rv64/Memory.hpp>

namespace rv64 {
    enum class VMState {
        Initializing,
        Running,
        Stopped,
        Error,
        Breakpoint,
        Finished
    };

    class VM {
    public:
        VM();

        void terminate(int exit_code);

        void error_stop();

        void breakpoint_hit();

        void set_program_start_address(uint64_t addr);

        void set_stack_start_address(uint64_t addr);

        struct Settings {
            uint64_t program_start_address = 0x400000;
            uint64_t stack_start_address = 0x7FFFFFF0;
            size_t stack_size = 1024 * 64; // 64 KiB
        };

        const Settings& get_settings() const noexcept;

    public:
        Memory m_memory;
        Cpu m_cpu{};

    private:
        Settings settings;
        VMState state = VMState::Initializing;
    };



} // rv64