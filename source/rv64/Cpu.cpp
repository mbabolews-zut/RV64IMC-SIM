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
        for (size_t i = 0; i < m_int_regs.size(); i++) {
            const auto& reg = m_int_regs[i];
            std::cout << std::format("[{:4} {:>5} = {:016X} (i64:{:<20}",
                                     reg.get_name() + "]",
                                     reg.get_sym_name(),
                                     reg.val(),
                                     std::to_string(reg.sval()) + ')');
            std::cout << (i % 2 == 0 ? " " : "\n");
        }
    }

    Cpu::Cpu() : m_int_regs(reg_array_construct(std::make_index_sequence<INT_REG_CNT>{})) {
    }
}
