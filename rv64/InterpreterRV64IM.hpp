#pragma once
#include <rv64/InstructionSets/IBase.hpp>
#include <rv64/InstructionSets/IExtM.hpp>

#include "Memory.hpp"

namespace rv64 {
    class Cpu;

class InterpreterRV64IM final
        : public is::IBase,
          public is::IExtM {
public:
    /// @copydoc is::IBase::addi
    void addi(IntReg &rd, const IntReg &rs, int12 imm12) override;

    /// @copydoc is::IBase::slti
    void slti(IntReg &rd, const IntReg &rs, int12 imm12) override;

    /// @copydoc is::IBase::sltiu
    void sltiu(IntReg &rd, const IntReg &rs, uint12 imm12) override;

    /// @copydoc is::IBase::andi
    void andi(IntReg &rd, const IntReg &rs, int12 imm12) override;

    /// @copydoc is::IBase::ori
    void ori(IntReg &rd, const IntReg &rs, int12 imm12) override;

    /// @copydoc is::IBase::xori
    void xori(IntReg &rd, const IntReg &rs, uint12 imm12) override;

    /// @copydoc is::IBase::slli
    void slli(IntReg &rd, const IntReg &rs, uint12 imm12) override;

    /// @copydoc is::IBase::srli
    void srli(IntReg &rd, const IntReg &rs, uint12 imm12) override;

    /// @copydoc is::IBase::srai
    void srai(IntReg &rd, const IntReg &rs, uint12 imm12) override;

    /// @copydoc is::IBase::lui
    void lui(IntReg &rd, int20 imm20) override;

    /// @copydoc is::IBase::auipc
    void auipc(IntReg &rd, int20 imm20) override;

    /// @copydoc is::IBase::add
    void add(IntReg &rd, const IntReg &rs1, const IntReg &rs2) override;

    /// @copydoc is::IBase::sub
    void sub(IntReg &rd, const IntReg &rs1, const IntReg &rs2) override;

    /// @copydoc is::IBase::slt
    void slt(IntReg &rd, const IntReg &rs1, const IntReg &rs2) override;

    /// @copydoc is::IBase::sltu
    void sltu(IntReg &rd, const IntReg &rs1, const IntReg &rs2) override;

    /// @copydoc is::IBase::and_
    void and_(IntReg &rd, const IntReg &rs1, const IntReg &rs2) override;

    /// @copydoc is::IBase::or_
    void or_(IntReg &rd, const IntReg &rs1, const IntReg &rs2) override;

    /// @copydoc is::IBase::xor_
    void xor_(IntReg &rd, const IntReg &rs1, const IntReg &rs2) override;

    /// @copydoc is::IBase::sll
    void sll(IntReg &rd, const IntReg &rs1, const IntReg &rs2) override;

    /// @copydoc is::IBase::srl
    void srl(IntReg &rd, const IntReg &rs1, const IntReg &rs2) override;

    /// @copydoc is::IBase::sra
    void sra(IntReg &rd, const IntReg &rs1, const IntReg &rs2) override;

    /// @copydoc is::IBase::jal
    void jal(IntReg &rd, int imm20) override;

    /// @copydoc is::IBase::jalr
    void jalr(IntReg &rd, const IntReg &rs, int12 imm12) override;

    /// @copydoc is::IBase::beq
    void beq(const IntReg &rs1, const IntReg &rs2, int imm12) override;

    /// @copydoc is::IBase::bne
    void bne(const IntReg &rs1, const IntReg &rs2, int imm12) override;

    /// @copydoc is::IBase::blt
    void blt(const IntReg &rs1, const IntReg &rs2, int12 imm12) override;

    /// @copydoc is::IBase::bltu
    void bltu(const IntReg &rs1, const IntReg &rs2, int12 imm12) override;

    /// @copydoc is::IBase::bge
    void bge(const IntReg &rs1, const IntReg &rs2, int12 imm12) override;

    /// @copydoc is::IBase::bgeu
    void bgeu(const IntReg &rs1, const IntReg &rs2, int12 imm12) override;

    /// @copydoc is::IBase::lw
    void lw(IntReg &rd, const IntReg &rs, int12 imm12) override;

    /// @copydoc is::IBase::lh
    void lh(IntReg &rd, const IntReg &rs, int12 imm12) override;

    /// @copydoc is::IBase::lhu
    void lhu(IntReg &rd, const IntReg &rs, int12 imm12) override;

    /// @copydoc is::IBase::lb
    void lb(IntReg &rd, const IntReg &rs, int12 imm12) override;

    /// @copydoc is::IBase::lbu
    void lbu(IntReg &rd, const IntReg &rs, int12 imm12) override;

    /// @copydoc is::IBase::sw
    void sw(const IntReg &rs, const IntReg &rs2, int12 imm12) override;

    /// @copydoc is::IBase::sh
    void sh(const IntReg &rs, const IntReg &rs2, int12 imm12) override;

    /// @copydoc is::IBase::sb
    void sb(const IntReg &rs, const IntReg &rs2, int12 imm12) override;

    /// @copydoc is::IBase::fence
    void fence() override;

    /// @copydoc is::IBase::ecall
    void ecall() override;

    /// @copydoc is::IBase::ebreak
    void ebreak() override;

    /// @copydoc is::IBase::addiw
    void addiw(IntReg &rd, const IntReg &rs, int12 imm12) override;

    /// @copydoc is::IBase::slliw
    void slliw(IntReg &rd, const IntReg &rs, uint5 imm5) override;

    /// @copydoc is::IBase::srliw
    void srliw(IntReg &rd, const IntReg &rs, uint5 imm5) override;

    /// @copydoc is::IBase::sraiw
    void sraiw(IntReg &rd, const IntReg &rs, uint5 imm5) override;

    /// @copydoc is::IBase::sllw
    void sllw(IntReg &rd, const IntReg &rs1, const IntReg &rs2) override;

    /// @copydoc is::IBase::srlw
    void srlw(IntReg &rd, const IntReg &rs1, const IntReg &rs2) override;

    /// @copydoc is::IBase::sraw
    void sraw(IntReg &rd, const IntReg &rs1, const IntReg &rs2) override;

    /// @copydoc is::IBase::addw
    void addw(IntReg &rd, const IntReg &rs1, const IntReg &rs2) override;

    /// @copydoc is::IBase::subw
    void subw(IntReg &rd, const IntReg &rs1, const IntReg &rs2) override;

    /// @copydoc is::IBase::ld
    void ld(IntReg &rd, const IntReg &rs, int12 imm12) override;

    /// @copydoc is::IBase::lwu
    void lwu(IntReg &rd, const IntReg &rs, int12 imm12) override;

    /// @copydoc is::IBase::sd
    void sd(const IntReg &rs, const IntReg &rs2, int12 imm12) override;

    /// @copydoc is::IExtM::mul
    void mul(IntReg &rd, const IntReg &rs1, const IntReg &rs2) override;

    /// @copydoc is::IExtM::mulh
    void mulh(IntReg &rd, const IntReg &rs1, const IntReg &rs2) override;

    /// @copydoc is::IExtM::mulhu
    void mulhu(IntReg &rd, const IntReg &rs1, const IntReg &rs2) override;

    /// @copydoc is::IExtM::mulhsu
    void mulhsu(IntReg &rd, const IntReg &rs1, const IntReg &rs2) override;

    /// @copydoc is::IExtM::mulw
    void mulw(IntReg &rd, const IntReg &rs1, const IntReg &rs2) override;

    /// @copydoc is::IExtM::div
    void div(IntReg &rd, const IntReg &rs1, const IntReg &rs2) override;

    /// @copydoc is::IExtM::divu
    void divu(IntReg &rd, const IntReg &rs1, const IntReg &rs2) override;

    /// @copydoc is::IExtM::rem
    void rem(IntReg &rd, const IntReg &rs1, const IntReg &rs2) override;

    /// @copydoc is::IExtM::remu
    void remu(IntReg &rd, const IntReg &rs1, const IntReg &rs2) override;

    /// @copydoc is::IExtM::divw
    void divw(IntReg &rd, const IntReg &rs1, const IntReg &rs2) override;

    /// @copydoc is::IExtM::divuw
    void divuw(IntReg &rd, const IntReg &rs1, const IntReg &rs2) override;

    /// @copydoc is::IExtM::remw
    void remw(IntReg &rd, const IntReg &rs1, const IntReg &rs2) override;

    /// @copydoc is::IExtM::remuw
    void remuw(IntReg &rd, const IntReg &rs1, const IntReg &rs2) override;

    ~InterpreterRV64IM() override;

private:
    template<typename T>
    void load_instruction_tmpl(IntReg &rd, const IntReg &rs, int12 imm12);

    void handle_error(MemErr err) const;

    Cpu &m_cpu;
};

}
