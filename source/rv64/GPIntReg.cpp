#include "GPIntReg.hpp"
#include "Cpu.hpp"
#include <cassert>
#include <format>

template<typename T>
T &get_zero_ref() {
    static T zero;
    zero = 0;
    return zero;
}

namespace rv64 {
    void Cpu::set_pc(uint64_t pc) {
        m_pc = pc & ~UINT64_C(1); // ensure LSB is 0
    }

    void Cpu::move_pc(int64_t offset) {
        m_pc += offset;
        m_pc &= ~UINT64_C(1); // ensure LSB is 0
    }

    GPIntReg &Cpu::reg(int i) noexcept {
        assert(i < INT_REG_CNT);
        return m_int_regs[i];
    }

    const GPIntReg &Cpu::reg(int i) const noexcept {
        assert(i < INT_REG_CNT);
        return m_int_regs[i];
    }

    GPIntReg & Cpu::reg(Reg reg) noexcept {
        return m_int_regs[reg.idx()];
    }

    const GPIntReg & Cpu::reg(Reg reg) const noexcept {
        return this->reg(reg.idx());
    }
}
