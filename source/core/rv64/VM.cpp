#include "VM.hpp"
#include <cassert>
#include <format>
#include <ui.hpp>

namespace rv64 {
    VM::VM(const Memory::Layout &layout)
        : m_memory(layout) {
        m_state = VMState::Initializing;
    }

    void VM::load_program(const asm_parsing::ParsedInstVec &instructions) {
        m_memory.load_program(instructions);
        m_cpu.set_pc(m_memory.get_layout().data_base);
        m_state = VMState::Loaded;
    }

    void VM::run_step() {
        assert(m_state == VMState::Loaded ||
            m_state == VMState::Running ||
            m_state == VMState::Stopped ||
            m_state == VMState::Breakpoint);

        m_state = VMState::Running;

        if (!m_cpu.next_cycle()) {
            m_state = VMState::Finished;
        }
    }

    void VM::run_until_stop() {
        assert(m_state == VMState::Loaded || m_state == VMState::Running);
        while (m_state == VMState::Running) {
            run_step();
        }
    }

    void VM::terminate(int exit_code) {
        m_state = VMState::Finished;
        ui::print_info(std::format("Program terminated with exit code {}", exit_code));
    }

    void VM::error_stop() {
        m_state = VMState::Error;
#ifdef TESTING
        throw std::runtime_error("VM encountered an error and stopped execution.");
#endif
    }

    void VM::breakpoint_hit() {
        m_state = VMState::Breakpoint;
    }

    void VM::reset() {
        m_state = VMState::Initializing;
        m_memory = Memory(m_memory.get_layout());
        m_cpu = Cpu(*this);
    }

    bool VM::toggle_breakpoint(size_t line) {
        bool has = m_cpu.has_breakpoint(line);
        m_cpu.set_breakpoint(line, !has);
        return !has;
    }

    bool VM::has_breakpoint(size_t line) const {
        return m_cpu.has_breakpoint(line);
    }

    void VM::clear_breakpoints() {
        m_cpu.clear_breakpoints();
    }

    bool VM::check_breakpoint() const {
        return m_cpu.has_breakpoint(get_current_line());
    }

    VMState VM::get_state() const noexcept {
        return m_state;
    }

    const Memory::Layout &VM::get_memory_layout() const noexcept {
        return m_memory.get_layout();
    }

    size_t VM::get_current_line() const noexcept { return m_cpu.m_interpreter.get_current_line(); }
} // rv64
