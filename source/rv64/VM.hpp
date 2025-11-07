#pragma once
#include <rv64/Cpu.hpp>
#include <rv64/Memory.hpp>
#include <parser/ParserProcessor.hpp>

namespace rv64 {
    enum class VMState {
        Initializing,
        Loaded,
        Running,
        Stopped,
        Error,
        Breakpoint,
        Finished
    };

    class VM {
    public:
        VM();

        void load_program(const asm_parsing::ParsedInstVec &instructions);

        void run_step();

        void run_until_stop();

        void terminate(int exit_code);

        void error_stop();

        void breakpoint_hit();

        void set_program_start_address(uint64_t addr);

        void set_stack_start_address(uint64_t addr);

        [[nodiscard]] VMState get_state() const noexcept;

        struct Settings {
            uint64_t prog_start_address = 0x400000;
            uint64_t stack_start_address = 0x7FFFFFF0;
            size_t stack_size = 1024 * 1024; // 1 MiB
            bool ascending_stack = false;
            bool big_endian = false;
        };

        [[nodiscard]] const Settings& get_settings() const noexcept;

    public:
        Memory m_memory;
        Cpu m_cpu{*this};

    private:
        Settings m_settings;
        VMState m_state = VMState::Initializing;
    };



} // rv64