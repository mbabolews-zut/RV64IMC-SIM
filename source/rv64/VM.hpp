#pragma once
#include <rv64/Cpu.hpp>
#include <rv64/Memory.hpp>
#include <parser/ParserProcessor.hpp>

namespace rv64 {
    /// Virtual machine execution states
    enum class VMState {
        Initializing,  ///< VM is being initialized
        Loaded,        ///< Program loaded, ready to execute
        Running,       ///< Program is executing
        Stopped,       ///< Execution paused
        Error,         ///< Error occurred during execution
        Breakpoint,    ///< Breakpoint hit
        Finished       ///< Program execution completed
    };

    /// Virtual machine encapsulating CPU and memory subsystems
    ///
    /// The VM class provides a complete RISC-V virtual machine implementation,
    /// managing the CPU state, memory subsystem, and execution lifecycle.
    /// It coordinates instruction fetch, decode, and execution while maintaining
    /// proper state transitions.
    class VM {
    public:
        /// Construct a virtual machine with specified memory layout
        /// @param layout Memory configuration (defaults to standard RISC-V layout)
        explicit VM(const Memory::Layout &layout = Memory::Layout{});

        /// Load program instructions into VM memory
        /// @param instructions Vector of parsed instructions to execute
        /// @note Sets program counter to data base address and transitions to Loaded state
        void load_program(const asm_parsing::ParsedInstVec &instructions);

        /// Execute a single instruction
        /// @note Transitions from Loaded/Running/Stopped to Running, then to Finished if program ends
        void run_step();

        /// Execute instructions until VM stops
        /// @note Continues execution while state is Running
        void run_until_stop();

        /// Terminate program execution with exit code
        /// @param exit_code Exit code to report
        /// @note Transitions to Finished state and prints termination message
        void terminate(int exit_code);

        /// Stop execution due to error
        /// @note Transitions to Error state
        void error_stop();

        /// Handle breakpoint hit
        /// @note Transitions to Breakpoint state
        void breakpoint_hit();

        /// Get current VM execution state
        /// @return Current VMState
        [[nodiscard]] VMState get_state() const noexcept;

        /// Get memory layout configuration
        /// @return Const reference to memory layout
        [[nodiscard]] const Memory::Layout &get_memory_layout() const noexcept;

    public:
        Memory m_memory;  ///< Memory subsystem (stack, heap, data)
        Cpu m_cpu{*this}; ///< CPU state and execution unit

    private:
        VMState m_state = VMState::Initializing; ///< Current execution state
    };


} // rv64