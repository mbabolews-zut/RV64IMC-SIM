#include "InterpreterRV64IM.hpp"
#include <rv64/Cpu.hpp>
#include <rv64/Memory.hpp>

namespace rv64 {
    void InterpreterRV64IM::addi(IntReg &rd, const IntReg &rs, int12 imm12) {
        rd = rs.sval() + imm12;
    }

    void InterpreterRV64IM::slti(IntReg &rd, const IntReg &rs, int12 imm12) {
        rd = rs.sval() < imm12 ? 1 : 0;
    }

    void InterpreterRV64IM::sltiu(IntReg &rd, const IntReg &rs, uint12 imm12) {
        rd = rs.val() < imm12 ? 1 : 0;
    }

    void InterpreterRV64IM::andi(IntReg &rd, const IntReg &rs, int12 imm12) {
        rd = rs.sval() & imm12;
    }

    void InterpreterRV64IM::ori(IntReg &rd, const IntReg &rs, int12 imm12) {
        rd = rs.sval() | imm12;
    }

    void InterpreterRV64IM::xori(IntReg &rd, const IntReg &rs, uint12 imm12) {
        rd = rs.sval() ^ imm12;
    }

    void InterpreterRV64IM::slli(IntReg &rd, const IntReg &rs, uint12 imm12) {
        rd = rs.val() << imm12;
    }

    void InterpreterRV64IM::srli(IntReg &rd, const IntReg &rs, uint12 imm12) {
        rd = rs.val() >> imm12;
    }

    void InterpreterRV64IM::srai(IntReg &rd, const IntReg &rs, uint12 imm12) {
        rd = rs.sval() >> imm12; // C++ 20 defined behaviour
    }

    void InterpreterRV64IM::lui(IntReg &rd, int20 imm20) {
        rd = m_cpu.get_pc() + (imm20 << 12);
    }
}
