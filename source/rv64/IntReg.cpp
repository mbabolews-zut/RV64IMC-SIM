#include "IntReg.hpp"
#include "Cpu.hpp"
#include <cassert>
#include <format>
#include <unordered_map>

static const std::unordered_map<std::string, int> sym_name_map = {
    {"zero", 0}, {"ra", 1},   {"sp", 2},   {"gp", 3},   {"tp", 4},
    {"t0", 5},   {"t1", 6},   {"t2", 7},   {"s0", 8},   {"fp", 8},
    {"s1", 9},   {"a0", 10},  {"a1", 11},  {"a2", 12},  {"a3", 13},
    {"a4", 14},  {"a5", 15},  {"a6", 16},  {"a7", 17},  {"s2", 18},
    {"s3", 19},  {"s4", 20},  {"s5", 21},  {"s6", 22},  {"s7", 23},
    {"s8", 24},  {"s9", 25},  {"s10", 26}, {"s11", 27}, {"t3", 28},
    {"t4", 29},  {"t5", 30},  {"t6", 31}, {"x0", 0},    {"x1", 1},
    {"x2", 2},   {"x3", 3},   {"x4", 4},   {"x5", 5},   {"x6", 6},
    {"x7", 7},   {"x8", 8},   {"x9", 9},   {"x10", 10}, {"x11", 11},
    {"x12", 12}, {"x13", 13}, {"x14", 14}, {"x15", 15}, {"x16", 16},
    {"x17", 17}, {"x18", 18}, {"x19", 19}, {"x20", 20}, {"x21", 21},
    {"x22", 22}, {"x23", 23}, {"x24", 24}, {"x25", 25}, {"x26", 26},
    {"x27", 27}, {"x28", 28}, {"x29", 29}, {"x30", 30}, {"x31", 31},
};

namespace rv64 {
    int32_t IntReg::as_i32() const {
        return std::bit_cast<int32_t>(as_u32());
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
        m_pc = pc & ~UINT64_C(1); // ensure LSB is 0
    }

    IntReg &Cpu::get_int_reg(size_t i) noexcept {
        assert(i < INT_REG_CNT);
        return m_int_regs[i];
    }

    const IntReg &Cpu::get_int_reg(size_t i) const noexcept {
        assert(i < INT_REG_CNT);
        return m_int_regs[i];
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

    IntReg::IntReg(size_t idx, bool owned_by_cpu) : m_idx(idx), m_owned_by_cpu(owned_by_cpu) {
        assert(idx < Cpu::INT_REG_CNT);
    }

    int IntReg::name_to_idx(const std::string &name) {
        auto it = sym_name_map.find(name);
        if (it == sym_name_map.end()) return -1;
        return it->second;
    }
}