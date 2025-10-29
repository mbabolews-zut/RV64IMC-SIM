#include "Cpu.hpp"
#include <cassert>

namespace rv64 {

    uint64_t Cpu::get_pc() const {
        assert(m_pc % 2 == 0);
        return m_pc;
    }

    Cpu::Cpu() : m_int_regs(reg_array_construct(std::make_index_sequence<INT_REG_CNT>{})) {
    }

}
