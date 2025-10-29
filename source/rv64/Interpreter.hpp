#pragma once

#include <unordered_map>
#include <rv64/Memory.hpp>
#include <rv64/instruction_sets/Rv64IMC.hpp>

namespace rv64 {
    class VM;
}

namespace rv64 {
    class Cpu;

class Interpreter final
        : public is::Rv64IMC {
public:
    explicit Interpreter(VM &vm) : m_vm(vm) {}

    /// @copydoc is::IBase::addi
    void addi(GPIntReg &rd, const GPIntReg &rs, int12 imm12) override;

    /// @copydoc is::IBase::slti
    void slti(GPIntReg &rd, const GPIntReg &rs, int12 imm12) override;

    /// @copydoc is::IBase::sltiu
    void sltiu(GPIntReg &rd, const GPIntReg &rs, int12 imm12) override;

    /// @copydoc is::IBase::andi
    void andi(GPIntReg &rd, const GPIntReg &rs, int12 imm12) override;

    /// @copydoc is::IBase::ori
    void ori(GPIntReg &rd, const GPIntReg &rs, int12 imm12) override;

    /// @copydoc is::IBase::xori
    void xori(GPIntReg &rd, const GPIntReg &rs, int12 imm12) override;

    /// @copydoc is::IBase::slli
    void slli(GPIntReg &rd, const GPIntReg &rs, uint6 uimm6) override;

    /// @copydoc is::IBase::srli
    void srli(GPIntReg &rd, const GPIntReg &rs, uint6 uimm6) override;

    /// @copydoc is::IBase::srai
    void srai(GPIntReg &rd, const GPIntReg &rs, uint6 uimm6) override;

    /// @copydoc is::IBase::lui
    void lui(GPIntReg &rd, int20 imm20) override;

    /// @copydoc is::IBase::auipc
    void auipc(GPIntReg &rd, int20 imm20) override;

    /// @copydoc is::IBase::add
    void add(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @copydoc is::IBase::sub
    void sub(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @copydoc is::IBase::slt
    void slt(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @copydoc is::IBase::sltu
    void sltu(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @copydoc is::IBase::and_
    void and_(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @copydoc is::IBase::or_
    void or_(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @copydoc is::IBase::xor_
    void xor_(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @copydoc is::IBase::sll
    void sll(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @copydoc is::IBase::srl
    void srl(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @copydoc is::IBase::sra
    void sra(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @copydoc is::IBase::jal
    void jal(GPIntReg &rd, int20 imm20) override;

    /// @copydoc is::IBase::jalr
    void jalr(GPIntReg &rd, const GPIntReg &rs, int12 imm12) override;

    /// @copydoc is::IBase::beq
    void beq(const GPIntReg &rs1, const GPIntReg &rs2, int12 imm12) override;

    /// @copydoc is::IBase::bne
    void bne(const GPIntReg &rs1, const GPIntReg &rs2, int12 imm12) override;

    /// @copydoc is::IBase::blt
    void blt(const GPIntReg &rs1, const GPIntReg &rs2, int12 imm12) override;

    /// @copydoc is::IBase::bltu
    void bltu(const GPIntReg &rs1, const GPIntReg &rs2, int12 imm12) override;

    /// @copydoc is::IBase::bge
    void bge(const GPIntReg &rs1, const GPIntReg &rs2, int12 imm12) override;

    /// @copydoc is::IBase::bgeu
    void bgeu(const GPIntReg &rs1, const GPIntReg &rs2, int12 imm12) override;

    /// @copydoc is::IBase::lw
    void lw(GPIntReg &rd, const GPIntReg &rs, int12 imm12) override;

    /// @copydoc is::IBase::lh
    void lh(GPIntReg &rd, const GPIntReg &rs, int12 imm12) override;

    /// @copydoc is::IBase::lhu
    void lhu(GPIntReg &rd, const GPIntReg &rs, int12 imm12) override;

    /// @copydoc is::IBase::lb
    void lb(GPIntReg &rd, const GPIntReg &rs, int12 imm12) override;

    /// @copydoc is::IBase::lbu
    void lbu(GPIntReg &rd, const GPIntReg &rs, int12 imm12) override;

    /// @copydoc is::IBase::sw
    void sw(const GPIntReg &rs, const GPIntReg &rs2, int12 imm12) override;

    /// @copydoc is::IBase::sh
    void sh(const GPIntReg &rs, const GPIntReg &rs2, int12 imm12) override;

    /// @copydoc is::IBase::sb
    void sb(const GPIntReg &rs, const GPIntReg &rs2, int12 imm12) override;

    /// @copydoc is::IBase::fence
    void fence() override;

    /// @copydoc is::IBase::ecall
    /// @brief ecall codes compatible with Venus simulator
    /// https://github.com/kvakil/venus/wiki/Environmental-Calls
    /// <br> a0 = 1 -> print_int - prints integer in a1.
    /// <br> a0 = 4 -> print_str - prints null-terminated string at address in a1.
    /// <br> a0 = 9 -> allocates a1 bytes on the heap, returns pointer to start in a0.
    /// <br> a0 = 10 -> exits program with return code 0.
    /// <br> a0 = 11 -> prints ASCII character in a1.
    /// <br> a0 = 17 -> exits program with return code in a1
    void ecall() override;

    /// @copydoc is::IBase::ebreak
    void ebreak() override;

    /// @copydoc is::IBase::addiw
    void addiw(GPIntReg &rd, const GPIntReg &rs, int12 imm12) override;

    /// @copydoc is::IBase::slliw
    void slliw(GPIntReg &rd, const GPIntReg &rs, uint5 imm5) override;

    /// @copydoc is::IBase::srliw
    void srliw(GPIntReg &rd, const GPIntReg &rs, uint5 imm5) override;

    /// @copydoc is::IBase::sraiw
    void sraiw(GPIntReg &rd, const GPIntReg &rs, uint5 imm5) override;

    /// @copydoc is::IBase::sllw
    void sllw(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @copydoc is::IBase::srlw
    void srlw(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @copydoc is::IBase::sraw
    void sraw(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @copydoc is::IBase::addw
    void addw(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @copydoc is::IBase::subw
    void subw(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @copydoc is::IBase::ld
    void ld(GPIntReg &rd, const GPIntReg &rs, int12 imm12) override;

    /// @copydoc is::IBase::lwu
    void lwu(GPIntReg &rd, const GPIntReg &rs, int12 imm12) override;

    /// @copydoc is::IBase::sd
    void sd(const GPIntReg &rs1, const GPIntReg &rs2, int12 imm12) override;

    /// @copydoc is::IExtM::mul
    void mul(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @copydoc is::IExtM::mulh
    void mulh(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @copydoc is::IExtM::mulhu
    void mulhu(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @copydoc is::IExtM::mulhsu
    void mulhsu(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @copydoc is::IExtM::mulw
    void mulw(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @copydoc is::IExtM::div
    void div(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @copydoc is::IExtM::divu
    void divu(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @copydoc is::IExtM::rem
    void rem(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @copydoc is::IExtM::remu
    void remu(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @copydoc is::IExtM::divw
    void divw(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @copydoc is::IExtM::divuw
    void divuw(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @copydoc is::IExtM::remw
    void remw(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @copydoc is::IExtM::remuw
    void remuw(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    ~Interpreter() override = default;

private:
    void exec_instruction(const Instruction &in);

    template<typename T>
    void load_instruction_tmpl(GPIntReg &rd, const GPIntReg &rs, int12 imm12);

    template<typename T>
    void store_instruction_tmpl(const GPIntReg &rs, const GPIntReg &rs2, int12 imm12);

    template<bool Rem = false, typename T>
    [[nodiscard]] static int64_t div_rem_tmpl(T lhs, T rhs);

    void handle_error(MemErr err) const;

    static uint64_t mul64x64_128high(uint64_t a, uint64_t b);

private:
    VM &m_vm;
};

}
