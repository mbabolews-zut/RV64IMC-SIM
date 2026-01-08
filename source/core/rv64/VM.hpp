#pragma once
#include <rv64/Cpu.hpp>
#include <Memory.hpp>
#include <parser/ParserProcessor.hpp>

namespace rv64 {
    // VM execution states
    enum class VMState {
        Initializing = 0,
        Loaded = 1,
        Running = 2,
        Stopped = 3,
        Error = 4,
        Breakpoint = 5,
        Finished = 6
    };

    enum class SpPos {
        Zero,
        StackBottom,
        StackTop
    };

    struct VMConfig {
        Memory::Layout m_mem_layout = Memory::Layout();
        SpPos m_sp_pos = SpPos::StackTop;
    };

    class VM {
    public:
        explicit VM(const VMConfig &config = {});

        void load_program(const asm_parsing::ParsedInstVec &instructions);
        void run_step();
        void run_until_stop();
        void terminate(int exit_code);
        void error_stop();
        void breakpoint_hit();
        void reset();

        void set_config(const VMConfig &config);
        [[nodiscard]] const VMConfig &get_config() const noexcept;


        /// @return True if breakpoint was added, false if removed
        bool toggle_breakpoint(size_t line);
        [[nodiscard]] bool has_breakpoint(size_t line) const;
        void clear_breakpoints();
        [[nodiscard]] bool check_breakpoint() const;

        [[nodiscard]] VMState get_state() const noexcept;
        [[nodiscard]] const Memory::Layout &get_memory_layout() const noexcept;
        [[nodiscard]] size_t get_current_line() const noexcept;


        Memory m_memory; // memory subsystem
        Cpu m_cpu{*this}; // CPU and interpreter

    private:
        VMConfig m_config;
        VMState m_state = VMState::Initializing;
    };
}
