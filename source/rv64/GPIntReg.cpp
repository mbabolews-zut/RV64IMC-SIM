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
    int32_t GPIntReg::as_i32() const {
        return std::bit_cast<int32_t>(as_u32());
    }

    uint32_t GPIntReg::as_u32() const {
        return static_cast<uint32_t>(val());
    }

    uint64_t GPIntReg::val() const {
        return m_value;
    }

    uint64_t &GPIntReg::val() {
        if (get_idx() == 0) return get_zero_ref<uint64_t>();
        return m_value;
    }

    int64_t GPIntReg::sval() const {
        return std::bit_cast<int64_t>(m_value);
    }

    int64_t &GPIntReg::sval() {
        if (get_idx() == 0) return get_zero_ref<int64_t>();
        return *reinterpret_cast<int64_t*>(&m_value);
    }


    void Cpu::set_pc(uint64_t pc) {
        m_pc = pc & ~UINT64_C(1); // ensure LSB is 0
    }

    GPIntReg &Cpu::reg(int i) noexcept {
        assert(i < INT_REG_CNT);
        return m_int_regs[i];
    }

    const GPIntReg &Cpu::get_int_reg(int i) const noexcept {
        assert(i < INT_REG_CNT);
        return m_int_regs[i];
    }

    GPIntReg & Cpu::get_int_reg(Reg reg) noexcept {
        return m_int_regs[reg.get_idx()];
    }

    const GPIntReg & Cpu::get_int_reg(Reg reg) const noexcept {
        return get_int_reg(reg.get_idx());
    }

    GPIntReg &GPIntReg::operator=(uint64_t val) {
        if (get_idx() != 0)
            m_value = val;
        return *this;
    }

    GPIntReg &GPIntReg::operator=(int64_t val) {
        if (get_idx() != 0)
            this->val() = std::bit_cast<uint64_t>(val);
        return *this;
    }

    GPIntReg &GPIntReg::operator=(int32_t val) {
        if (get_idx() != 0)
            m_value = std::bit_cast<uint64_t>(static_cast<int64_t>(val));
        return *this;
    }
}
