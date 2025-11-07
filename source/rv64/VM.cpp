#include "VM.hpp"
#include <cassert>
#include <format>

#include "../common.hpp"

namespace rv64 {

    VM::VM() {
        assert(m_state == VMState::Initializing);
        m_state = VMState::Initializing;
        std::array<uint8_t, 4> empty_prog{};
        m_memory.init(empty_prog);
    }

    void VM::load_program(const asm_parsing::ParsedInstVec &instructions) {
        m_memory.load_program(instructions);
        m_cpu.set_pc(m_settings.prog_start_address);
        m_state = VMState::Loaded;
    }

    void VM::run_step() {
        assert(m_state == VMState::Loaded || m_state == VMState::Running || m_state == VMState::Stopped);
        m_state = VMState::Running;

        // exec next instruction
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
    }

    void VM::breakpoint_hit() {
        m_state = VMState::Breakpoint;
    }

    void VM::set_program_start_address(uint64_t addr) {
        assert(m_state == VMState::Initializing);
        m_settings.prog_start_address = addr;
    }

    void VM::set_stack_start_address(uint64_t addr) {
        assert(m_state == VMState::Initializing);
        m_settings.stack_start_address = addr;
    }

    VMState VM::get_state() const noexcept {
        return m_state;
    }

    const VM::Settings & VM::get_settings() const noexcept {
        return m_settings;
    }
} // rv64