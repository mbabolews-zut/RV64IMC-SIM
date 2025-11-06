#include "Cpu.hpp"
#include <cassert>
#include <format>

namespace rv64 {

    uint64_t Cpu::get_pc() const {
        assert(m_pc % 2 == 0);
        return m_pc;
    }

    void Cpu::print_cpu_state() {
        std::cout << std::format("pc = {}\n", get_pc());
        for (const auto &reg : m_int_regs) {
            std::cout << std::format("{} = {:0X} (i64:{} u64:{})\n",
                reg.get_sym_name(), reg.val(), reg.sval(), reg.val());
        }
    }

    Cpu::Cpu() : m_int_regs(reg_array_construct(std::make_index_sequence<INT_REG_CNT>{})) {
    }

}
