#include "VM.hpp"
#include <cassert>

namespace rv64 {
    void VM::set_program_start_address(uint64_t addr) {
        assert(state == VMState::Initializing);
        settings.program_start_address = addr;
    }

    void VM::set_stack_start_address(uint64_t addr) {
        assert(state == VMState::Initializing);
        settings.stack_start_address = addr;
    }
} // rv64