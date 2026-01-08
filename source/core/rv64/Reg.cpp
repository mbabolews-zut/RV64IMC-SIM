#include "Reg.hpp"
#include <format>
#include <cassert>
#include <unordered_map>

#include "Cpu.hpp"

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
    std::string Reg::get_name() const {
        return std::format("x{}", m_idx);
    }

    std::string Reg::get_abi_name() const {
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
        return "unknown";
    }

    Reg::Reg(int idx) : m_idx(idx) {
        assert(idx < Cpu::INT_REG_CNT);
    }

    Reg::Reg(std::string_view name) : m_idx(name_to_idx(std::string(name))) {
    }

    int Reg::name_to_idx(const std::string &name) {
        auto it = sym_name_map.find(name);
        if (it == sym_name_map.end()) return -1;
        return it->second;
    }

    int Reg::idx() const {
        return m_idx;
    }

    bool Reg::in_compressed_range() const {
        return idx() >= 8 && idx() <= 15;
    }

    bool Reg::is_valid() const {
        return m_idx != -1;
    }

    Reg::operator bool() const {
        return is_valid();
    }
}
