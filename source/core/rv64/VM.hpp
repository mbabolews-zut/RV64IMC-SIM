#pragma once
#include <rv64/Cpu.hpp>
#include <Memory.hpp>
#include <parser/ParserProcessor.hpp>

namespace rv64 {
    // VM execution states
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
        explicit VM(const Memory::Layout &layout = Memory::Layout{});

        void load_program(const asm_parsing::ParsedInstVec &instructions);
        void run_step();
        void run_until_stop();
        void terminate(int exit_code);
        void error_stop();
        void breakpoint_hit();
        void reset();

        /// @return True if breakpoint was added, false if removed
        bool toggle_breakpoint(size_t line);
        [[nodiscard]] bool has_breakpoint(size_t line) const;
        void clear_breakpoints();
        [[nodiscard]] bool check_breakpoint() const;

        [[nodiscard]] VMState get_state() const noexcept;
        [[nodiscard]] const Memory::Layout &get_memory_layout() const noexcept;
        [[nodiscard]] size_t get_current_line() const noexcept;

    public:
        Memory m_memory; // memory subsystem
        Cpu m_cpu{*this}; // CPU and interpreter

    private:
        VMState m_state = VMState::Initializing;
    };
}
