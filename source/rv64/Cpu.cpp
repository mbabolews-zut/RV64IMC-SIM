#include "Cpu.hpp"
#include <cassert>
#include <format>

#include "VM.hpp"

namespace rv64 {
    uint64_t Cpu::get_pc() const {
        assert(m_pc % 2 == 0);
        return m_pc;
    }

    void Cpu::print_cpu_state() const {
        std::cout << std::format("pc = 0x{:016X}\n", get_pc());
        for (size_t i = 0; i < m_int_regs.size(); i++) {
            const auto &reg = m_int_regs[i];
            std::cout << std::format("[{:4} {:>5} = 0x{:016X} (i64:{:<20}",
                                     reg.get_name() + "]",
                                     reg.get_sym_name(),
                                     reg.val(),
                                     std::to_string(reg.sval()) + ')');
            std::cout << (i % 2 == 0 ? " " : "\n");
        }
    }

    bool Cpu::next_cycle() {
        MemErr mem_err;
        auto instruction = m_vm.m_memory.get_instruction_at(get_pc(), mem_err);
        if (mem_err != MemErr::None) {
            if (mem_err != MemErr::ProgramExit) {
                ui::display_error(Memory::err_to_string(mem_err));
                m_vm.error_stop();
            }
            return false;
        }
        m_interpreter.exec_instruction(instruction);
        m_pc += 4; // TODO: support for compressed instructions
        return m_vm.m_memory.get_instruction_end_addr() != m_pc;
    }

    Cpu::Cpu(VM &vm)
        : m_int_regs(reg_array_construct(std::make_index_sequence<INT_REG_CNT>{})),
          m_interpreter(vm), m_vm(vm) {
    }
}
