#include "Cpu.hpp"
#include <cassert>
#include <format>

namespace rv64 {
    int32_t IntReg::as_i32() const {
        return std::bit_cast<int32_t>(static_cast<uint32_t>(val()));
    }

    uint32_t IntReg::as_u32() const {
        return static_cast<uint32_t>(val());
    }

    uint64_t IntReg::val() const {
        return m_value;
    }

    uint64_t &IntReg::val() {
        return m_value;
    }

    int64_t IntReg::sval() const {
        return std::bit_cast<int64_t>(m_value);
    }

    int64_t &IntReg::sval() {
        return reinterpret_cast<int64_t &>(m_value);
    }


    void Cpu::set_pc(uint64_t pc) {
        m_pc = pc;
    }

    IntReg &Cpu::get_int_reg(size_t i) noexcept {
        assert(i < INT_REG_CNT);
        return m_int_regs[i];
    }

    const IntReg &Cpu::get_int_reg(size_t i) const noexcept {
        assert(i < INT_REG_CNT);
        return m_int_regs[i];
    }

    uint64_t Cpu::get_pc() const {
        return m_pc;
    }

    Cpu::Cpu() : m_int_regs(reg_array_construct<IntReg>(std::make_index_sequence<INT_REG_CNT>{})) {
    }

    std::string IntReg::get_name() const {
        return std::format("x{}", m_idx);
    }

    std::string IntReg::get_sym_name() const {
        switch (m_idx) {
            case 0: return "zero";
            case 1: return "ra";
            case 2: return "sp";
            case 3: return "gp";
            case 4: return "tp";
            case 5: return "t0";
            case 6: return "t1";
            case 7: return "t2";
            case 8: return "s0/fp";
            case 9: return "s1";
            default:
                if (m_idx < 18)
                    return std::format("a{}", m_idx - 10);
                if (m_idx < 28)
                    return std::format("s{}", m_idx - 16);
                if (m_idx < 36)
                    return std::format("t{}", m_idx - 25);
                assert(false && "unknown register");
        }
    }

    IntReg &IntReg::operator=(uint64_t val) {
        if (m_idx != 0)
            m_value = val;
        return *this;
    }

    IntReg &IntReg::operator=(int64_t val) {
        if (m_idx != 0)
            m_value = std::bit_cast<uint64_t>(val);
        return *this;
    }

    IntReg &IntReg::operator=(int32_t val) {
        if (m_idx != 0)
            m_value = std::bit_cast<uint64_t>(static_cast<int64_t>(val));
        return *this;
    }

    IntReg::IntReg(size_t idx) : m_idx(idx) {
        assert(idx < Cpu::INT_REG_CNT);
    }
}
