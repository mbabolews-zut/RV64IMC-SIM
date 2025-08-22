#include "InterpreterRV64IM.hpp"

#include <cassert>
#include <format>
#include <rv64/Cpu.hpp>
#include <rv64/Memory.hpp>

namespace rv64 {
    template void InterpreterRV64IM::load_instruction_tmpl<int64_t>(IntReg &rd, const IntReg &rs, int12 imm12);
    template void InterpreterRV64IM::load_instruction_tmpl<uint32_t>(IntReg &rd, const IntReg &rs, int12 imm12);
    template void InterpreterRV64IM::load_instruction_tmpl<uint16_t>(IntReg &rd, const IntReg &rs, int12 imm12);
    template void InterpreterRV64IM::load_instruction_tmpl<int16_t>(IntReg &rd, const IntReg &rs, int12 imm12);
    template void InterpreterRV64IM::load_instruction_tmpl<uint8_t>(IntReg &rd, const IntReg &rs, int12 imm12);
    template void InterpreterRV64IM::load_instruction_tmpl<int8_t>(IntReg &rd, const IntReg &rs, int12 imm12);


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
        rd = imm20.raw() << 12;
    }

    void InterpreterRV64IM::auipc(IntReg &rd, int20 imm20) {
        rd = m_cpu.get_pc() + (imm20.raw() << 12);
    }

    void InterpreterRV64IM::add(IntReg &rd, const IntReg &rs1, const IntReg &rs2) {
        rd = rs1.sval() + rs2.sval();
    }

    void InterpreterRV64IM::sub(IntReg &rd, const IntReg &rs1, const IntReg &rs2) {
        rd = rs1.sval() - rs2.sval();
    }

    void InterpreterRV64IM::slt(IntReg &rd, const IntReg &rs1, const IntReg &rs2) {
        rd = rs1.sval() < rs2.sval() ? 1 : 0;
    }

    void InterpreterRV64IM::sltu(IntReg &rd, const IntReg &rs1, const IntReg &rs2) {
        rd = rs1.val() < rs2.val() ? 1 : 0;
    }

    void InterpreterRV64IM::and_(IntReg &rd, const IntReg &rs1, const IntReg &rs2) {
        rd = rs1.val() & rs2.val();
    }

    void InterpreterRV64IM::or_(IntReg &rd, const IntReg &rs1, const IntReg &rs2) {
        rd = rs1.val() | rs2.val();
    }

    void InterpreterRV64IM::xor_(IntReg &rd, const IntReg &rs1, const IntReg &rs2) {
        rd = rs1.val() ^ rs2.val();
    }

    void InterpreterRV64IM::sll(IntReg &rd, const IntReg &rs1, const IntReg &rs2) {
        rd = rs1.val() << (rs2.val() & 0x3F);
    }

    void InterpreterRV64IM::srl(IntReg &rd, const IntReg &rs1, const IntReg &rs2) {
        rd = rs1.val() >> (rs2.val() & 0x3F);
    }

    void InterpreterRV64IM::sra(IntReg &rd, const IntReg &rs1, const IntReg &rs2) {
        rd = rs1.sval() >> (rs2.val() & 0x3F); // C++ 20 defined behaviour
    }

    void InterpreterRV64IM::jal(IntReg &rd, int imm20) {
        assert(imm20 % 4 == 0 && "unaligned jump");
        rd = m_cpu.get_pc() + 4;
        m_cpu.set_pc(m_cpu.get_pc() + imm20 - 4);
    }

    void InterpreterRV64IM::jalr(IntReg &rd, const IntReg &rs, int12 imm12) {
        rd = m_cpu.get_pc() + 4;
        m_cpu.set_pc(rs.sval() + imm12 - 4);
    }

    void InterpreterRV64IM::beq(const IntReg &rs1, const IntReg &rs2, int imm12) {
        // TODO: imm12 range assertion
        if (rs1.sval() == rs2.sval()) {
            m_cpu.set_pc(m_cpu.get_pc() + imm12 - 4);
        }
    }

    void InterpreterRV64IM::bne(const IntReg &rs1, const IntReg &rs2, int imm12) {
        if (rs1.sval() != rs2.sval()) {
            m_cpu.set_pc(m_cpu.get_pc() + imm12 - 4);
        }
    }

    void InterpreterRV64IM::blt(const IntReg &rs1, const IntReg &rs2, int12 imm12) {
        if (rs1.sval() < rs2.sval()) {
            m_cpu.set_pc(m_cpu.get_pc()+ imm12 - 4);
        }
    }

    void InterpreterRV64IM::bltu(const IntReg &rs1, const IntReg &rs2, int12 imm12) {
        if (rs1.val() < rs2.val()) {
            m_cpu.set_pc(m_cpu.get_pc() + imm12 - 4);
        }
    }

    void InterpreterRV64IM::bge(const IntReg &rs1, const IntReg &rs2, int12 imm12) {
        if (rs1.sval() >= rs2.sval()) {
            m_cpu.set_pc(m_cpu.get_pc() + imm12 - 4);
        }
    }

    void InterpreterRV64IM::bgeu(const IntReg &rs1, const IntReg &rs2, int12 imm12) {
        if (rs1.val() >= rs2.val()) {
            m_cpu.set_pc(m_cpu.get_pc() + imm12 - 4);
        }
    }

    void InterpreterRV64IM::lw(IntReg &rd, const IntReg &rs, int12 imm12) {
        load_instruction_tmpl<int32_t>(rd, rs, imm12);
    }

    void InterpreterRV64IM::lh(IntReg &rd, const IntReg &rs, int12 imm12) {
        load_instruction_tmpl<int16_t>(rd, rs, imm12);
    }

    void InterpreterRV64IM::lhu(IntReg &rd, const IntReg &rs, int12 imm12) {
        load_instruction_tmpl<uint16_t>(rd, rs, imm12);
    }

    void InterpreterRV64IM::lb(IntReg &rd, const IntReg &rs, int12 imm12) {
        load_instruction_tmpl<int8_t>(rd, rs, imm12);
    }

    void InterpreterRV64IM::lbu(IntReg &rd, const IntReg &rs, int12 imm12) {
        load_instruction_tmpl<uint8_t>(rd, rs, imm12);
    }

    void InterpreterRV64IM::sw(const IntReg &rs, const IntReg &rs2, int12 imm12) {
        auto err = Memory::store<uint32_t>(rs.sval() + imm12, static_cast<uint32_t>(rs2.val()));
        if (err != MemErr::None)
            handle_error(err);
    }

    void InterpreterRV64IM::handle_error(MemErr err) const {
        assert(err != MemErr::None);
        gui::display_error("Memory access error: " + Memory::err_to_string(err));
        m_cpu.on_error_stop();
    }

    template<typename T>
    void InterpreterRV64IM::load_instruction_tmpl(IntReg &rd, const IntReg &rs, int12 imm12) {
        MemErr err;
        rd = Memory::load<T>(rs.sval() + imm12, err);
        if (err != MemErr::None)
            handle_error(err);
    }


}
