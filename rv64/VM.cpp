#include "VM.hpp"
#include <cassert>
#include <format>

#include "common.hpp"

namespace rv64 {
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
        settings.program_start_address = addr;
    }

    void VM::set_stack_start_address(uint64_t addr) {
        assert(state == VMState::Initializing);
        settings.stack_start_address = addr;
    }
} // rv64