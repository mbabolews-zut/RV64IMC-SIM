#include "VM.hpp"
#include <cassert>
#include <format>

#include "../common.hpp"

namespace rv64 {

    VM::VM() {
        assert(state == VMState::Initializing);
        state = VMState::Initializing;
        std::array<uint8_t, 4> empty_prog{};
        m_memory.init(empty_prog);
    }

    void VM::terminate(int exit_code) {
        state = VMState::Finished;
        gui::print_info(std::format("Program terminated with exit code {}", exit_code));
    }

    void VM::error_stop() {
        state = VMState::Error;
    }

    void VM::breakpoint_hit() {
        state = VMState::Breakpoint;
    }

    void VM::set_program_start_address(uint64_t addr) {
        assert(state == VMState::Initializing);
        settings.prog_start_address = addr;
    }

    void VM::set_stack_start_address(uint64_t addr) {
        assert(state == VMState::Initializing);
        settings.stack_start_address = addr;
    }

    const VM::Settings & VM::get_settings() const noexcept {
        return settings;
    }
} // rv64