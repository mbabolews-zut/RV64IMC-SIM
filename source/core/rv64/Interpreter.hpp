#pragma once

#include <unordered_map>
#include <Memory.hpp>
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

    // current source line (moved from Cpu)
    [[nodiscard]] size_t get_current_line() const noexcept { return m_current_line; }
    void set_current_line(size_t ln) noexcept { m_current_line = ln; }

    //
    // ------- Integer Base Instructions (I) -------
    //

    /// @brief add immediate.
    /// <br> rd = rs + imm12
    void addi(GPIntReg &rd, const GPIntReg &rs, int12 imm12) override;

    /// @brief set less than immediate.
    /// <br> rd = signed(rs) < imm12 ? 1 : 0
    void slti(GPIntReg &rd, const GPIntReg &rs, int12 imm12) override;

    /// @brief set less than immediate (unsigned).
    /// <br> rd = rs < imm ? 1 : 0
    void sltiu(GPIntReg &rd, const GPIntReg &rs, int12 imm12) override;

    /// @brief logical AND immediate
    /// <br> rd = rs & imm12
    void andi(GPIntReg &rd, const GPIntReg &rs, int12 imm12) override;

    /// @brief logical OR immediate
    /// <br> rd = rs | imm12
    void ori(GPIntReg &rd, const GPIntReg &rs, int12 imm12) override;

    /// @brief logical XOR immediate
    /// <br> rd = rs ^ imm12
    void xori(GPIntReg &rd, const GPIntReg &rs, int12 imm12) override;

    /// @brief shift left logical immediate
    /// <br> rd = rs << imm12
    void slli(GPIntReg &rd, const GPIntReg &rs, uint6 uimm6) override;

    /// @brief shift right logical immediate
    /// <br> rd = rs >> imm12
    void srli(GPIntReg &rd, const GPIntReg &rs, uint6 uimm6) override;

    /// @brief shift right arithmetic immediate (the original sign bit is copied into the vacated upper bits).
    /// <br> rd = signed(rs) >> imm12
    void srai(GPIntReg &rd, const GPIntReg &rs, uint6 uimm6) override;

    /// @brief load upper immediate
    /// <br> rd = sign_extend<i64>(imm20 << 12)
    void lui(GPIntReg &rd, int20 imm20) override;

    /// @brief add upper immediate to pc
    /// <br> rd = pc + sign_extend<i64>(imm20 << 12)
    void auipc(GPIntReg &rd, int20 imm20) override;

    /// @brief add two registers. (signed)
    /// <br> rd = rs1 + rs2
    void add(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @brief subtract two registers. (signed)
    /// <br> rd = rs1 - rs2
    void sub(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @brief set less than. (signed)
    /// <br> rd = rs1 < rs2 ? 1 : 0
    void slt(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @brief set less than. (unsigned)
    /// <br> rd = rs1 < rs2 ? 1 : 0
    void sltu(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @brief logical AND
    /// <br> rd = rs1 & rs2
    void and_(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @brief logical OR
    /// <br> rd = rs1 | rs2
    void or_(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @brief logical XOR
    /// <br> rd = rs1 ^ rs2
    void xor_(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @brief shift left logical
    /// <br> rd = rs1 << rs2[5:0] (the lower 6 bits of rs2)
    void sll(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @brief shift right logical
    /// <br> rd = rs1 >> rs2[5:0] (the lower 6 bits of rs2)
    void srl(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @brief shift right arithmetic (the original sign bit is copied into the vacated upper bits).
    /// <br> rd = signed(rs1) >> rs2[5:0] (the lower 6 bits of rs2)
    void sra(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @brief jump and link.
    /// <br> rd = pc + 4; goto pc + imm20
    void jal(GPIntReg &rd, int20 imm20) override;

    /// @brief jump and link register.
    /// <br> rd = pc + 4; pc = rs + imm12
    void jalr(GPIntReg &rd, const GPIntReg &rs, int12 imm12) override;

    /// @brief branch if equal.
    /// <br> pc += (rs1 == rs2) ? (imm12 << 1) : 4
    void beq(const GPIntReg &rs1, const GPIntReg &rs2, int12 imm12) override;

    /// @brief branch if not equal.
    /// <br> pc += (rs1 != rs2) ? (imm12 << 1) : 4
    void bne(const GPIntReg &rs1, const GPIntReg &rs2, int12 imm12) override;

    /// @brief branch if less than (signed).
    /// <br> pc += (rs1 < rs2) ? (imm12 << 1) : 4
    void blt(const GPIntReg &rs1, const GPIntReg &rs2, int12 imm12) override;

    /// @brief branch if less than (unsigned).
    /// <br> pc += (rs1 < rs2) ? (imm12 << 1) : 4
    void bltu(const GPIntReg &rs1, const GPIntReg &rs2, int12 imm12) override;

    /// @brief branch if greater than or equal (signed).
    /// <br> pc += (rs1 >= rs2) ? (imm12 << 1) : 4
    void bge(const GPIntReg &rs1, const GPIntReg &rs2, int12 imm12) override;

    /// @brief branch if greater than or equal (unsigned).
    /// <br> pc += (rs1 >= rs2) ? (imm12 << 1) : 4
    void bgeu(const GPIntReg &rs1, const GPIntReg &rs2, int12 imm12) override;

    /// @brief load 32-bit word from memory and sign-extend it to 64-bit.
    /// <br> rd = sign_extend<i64>(((i32*)mem)[rs + imm12])
    void lw(GPIntReg &rd, const GPIntReg &rs, int12 imm12) override;

    /// @brief load 16-bit halfword from memory and sign-extend it to 64-bit.
    /// <br> rd = sign_extend<i64>(((i16*)mem)[rs + imm12])
    void lh(GPIntReg &rd, const GPIntReg &rs, int12 imm12) override;

    /// @brief load 16-bit halfword from memory and zero-extend it to 64-bit.
    /// <br> rd = ((u16*)mem)[rs + imm12]
    void lhu(GPIntReg &rd, const GPIntReg &rs, int12 imm12) override;

    /// @brief load 8-bit byte from memory and sign-extend it to 64-bit.
    /// <br> rd = sign_extend<i64>(((i8*)mem)[rs + imm12])
    void lb(GPIntReg &rd, const GPIntReg &rs, int12 imm12) override;

    /// @brief load 8-bit byte from memory and zero-extend it to 64-bit.
    /// <br> rd = ((u8*)mem)[rs + imm12]
    void lbu(GPIntReg &rd, const GPIntReg &rs, int12 imm12) override;

    /// @brief store 32-bit word to memory.
    /// <br> ((u32*)mem)[rs + imm12] = rs2[0:31] (the lower 32 bits of rs2)
    void sw(const GPIntReg &rs, const GPIntReg &rs2, int12 imm12) override;

    /// @brief store 16-bit halfword to memory.
    /// <br> ((u16*)mem)[rs + imm12] = rs2[0:15] (the lower 16 bits of rs2)
    void sh(const GPIntReg &rs, const GPIntReg &rs2, int12 imm12) override;

    /// @brief store 8-bit byte to memory.
    /// <br> ((u8*)mem)[rs + imm12] = rs2[0:7] (the lower 8 bits of rs2)
    void sb(const GPIntReg &rs, const GPIntReg &rs2, int12 imm12) override;

    /// @brief fence instruction. https://en.wikipedia.org/wiki/Memory_barrier
    /// <br> does nothing on single-threaded systems.
    void fence() override;

    /// @brief Environmental call. ecall codes compatible with Venus simulator
    /// https://github.com/kvakil/venus/wiki/Environmental-Calls
    /// <br> a0 = 1 -> print_int - prints integer in a1.
    /// <br> a0 = 4 -> print_str - prints null-terminated string at address in a1.
    /// <br> a0 = 9 -> allocates a1 bytes on the heap, returns pointer to start in a0.
    /// <br> a0 = 10 -> exits program with return code 0.
    /// <br> a0 = 11 -> prints ASCII character in a1.
    /// <br> a0 = 17 -> exits program with return code in a1
    void ecall() override;

    /// @brief breakpoint instruction used for debugging.
    void ebreak() override;

    /// @brief adds imm12 to 32-bit register and sign-extends the result to 64 bits.
    /// <br> rd = sign_extend<i64>(rs[31:0] + signed(imm12))
    void addiw(GPIntReg &rd, const GPIntReg &rs, int12 imm12) override;

    /// @brief logical left shift that operates on a 32-bit register and sign-extends the result to 64 bits.
    /// <br> rd = sign_extend<i64>(rs[31:0] << imm5)
    void slliw(GPIntReg &rd, const GPIntReg &rs, uint5 imm5) override;

    /// @brief logical right shift that operates on a 32-bit register and sign-extends the result to 64 bits.
    /// <br> rd = sign_extend<i64>(rs[31:0] >> imm5)
    void srliw(GPIntReg &rd, const GPIntReg &rs, uint5 imm5) override;

    /// @brief arithmetic right shift that operates on a 32-bit register and sign-extends the result to 64 bits.
    /// <br> rd = sign_extend<i64>(signed(rs[31:0]) >> imm5)
    void sraiw(GPIntReg &rd, const GPIntReg &rs, uint5 imm5) override;

    /// @brief logical left shift that operates on a 32-bit register and sign-extends the result to 64 bits.
    /// <br> rd = sign_extend<i64>(rs1[31:0] << rs2[4:0])
    void sllw(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @brief logical right shift that operates on a 32-bit register and sign-extends the result to 64 bits.
    /// <br> rd = sign_extend<i64>(rs1[31:0] >> rs2[4:0])
    void srlw(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @brief arithmetic right shift that operates on a 32-bit register and sign-extends the result to 64 bits.
    /// <br> rd = sign_extend<i64>(signed(rs1[31:0]) >> rs2[4:0])
    void sraw(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @brief add 32-bit registers and sign-extend the result to 64 bits.
    /// <br> rd = sign_extend<i64>(rs1[31:0] + rs2[31:0])
    void addw(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @brief subtract 32-bit registers and sign-extend the result to 64 bits.
    /// <br> rd = sign_extend<i64>(rs1[31:0] - rs2[31:0])
    void subw(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @brief load 64-bit doubleword from memory.
    /// <br> rd = ((i64*)mem)[rs + imm12]
    void ld(GPIntReg &rd, const GPIntReg &rs, int12 imm12) override;

    /// @brief load 32-bit word from memory and zero-extend it to 64 bits.
    /// <br> rd = ((u32*)mem)[rs + imm12]
    void lwu(GPIntReg &rd, const GPIntReg &rs, int12 imm12) override;

    /// @brief store 64-bit doubleword to memory.
    /// <br> ((u64*)mem)[rs + imm12] = rs2
    void sd(const GPIntReg &rs1, const GPIntReg &rs2, int12 imm12) override;

    //
    // ------- Integer Multiplication and Division Instructions (M) -------
    //

    /// @brief multiply
    /// <br> rd = rs1 * rs2
    void mul(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @brief multiply, store 128-bit upper half in rd (signed registers)
    /// <br> i128 t = rs1 * rs2;
    /// <br> rd = t[127:64]
    void mulh(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @brief multiply, store 128-bit upper half in rd (unsigned registers)
    /// <br> u128 t = rs1 * rs2;
    /// <br> rd = t[127:64]
    void mulhu(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @brief multiply, store 128-bit upper half in rd (rs1 - signed, rs2 - unsigned)
    /// <br> i128 t = signed(rs1) * unsigned(rs2);
    /// <br> rd = t[127:64]
    void mulhsu(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @brief multiply 32-bit registers and sign-extend the result to 64 bits.
    /// <br> rd = sign_extend<i64>(rs1[31:0] * rs2[31:0])
    void mulw(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @brief divide (signed registers)
    /// <br> rd = rs1 / rs2
    void div(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @brief divide (unsigned registers)
    /// <br> rd = rs1 / rs2
    void divu(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @brief remainder (signed registers)
    /// <br> rd = rs1 % rs2
    void rem(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @brief remainder (unsigned registers)
    /// <br> rd = rs1 % rs2
    void remu(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @brief divide 32-bit registers and sign-extend the result to 64 bits.
    /// <br> rd = sign_extend<i64>(rs1[31:0] / rs2[31:0])
    void divw(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @brief divide 32-bit registers and zero-extend the result to 64 bits. (unsigned registers)
    /// <br> rd = sign_extend<i64>(rs1[31:0] / rs2[31:0])
    void divuw(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @brief remainder 32-bit registers and sign-extend the result to 64 bits. (signed registers)
    /// <br> rd = sign_extend<i64>(rs1[31:0] % rs2[31:0])
    void remw(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    /// @brief remainder 32-bit registers and zero-extend the result to 64 bits. (unsigned registers)
    /// <br> rd = sign_extend<i64>(rs1[31:0] % rs2[31:0])
    void remuw(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) override;

    //
    // ------- Compressed Base Instructions (C) -------
    //
    void c_lwsp(GPIntReg &rd, int6 imm6) override;
    void c_ldsp(GPIntReg &rd, int6 imm6) override;
    void c_fldsp(GPIntReg &rd, int6 imm6) override;
    void c_swsp(const GPIntReg &rs2, int6 imm6) override;
    void c_sdsp(const GPIntReg &rs2, int6 imm6) override;
    void c_fsdsp(GPIntReg &rd, int6 imm6) override;
    void c_lw(GPIntReg &rdp, const GPIntReg &rs1p, int5 imm5) override;
    void c_ld(GPIntReg &rdp, const GPIntReg &rs1p, int5 imm5) override;
    void c_fld(GPIntReg &rdp, const GPIntReg &rs1p, int5 imm5) override;
    void c_sw(const GPIntReg &rs2p, const GPIntReg &rs1p, int5 imm5) override;
    void c_sd(const GPIntReg &rs2p, const GPIntReg &rs1p, int5 imm5) override;
    void c_fsd(const GPIntReg &rs2p, const GPIntReg &rs1p, int5 imm5) override;
    void c_j(int11 imm11) override;
    void c_jr(const GPIntReg &rs1) override;
    void c_jalr(const GPIntReg &rs1) override;
    void c_beqz(const GPIntReg &rs1p, int8 imm8) override;
    void c_bnez(const GPIntReg &rs1p, int8 imm8) override;
    void c_li(GPIntReg &rd, int6 imm6) override;
    void c_lui(GPIntReg &rd, int6 nzimm6) override;
    void c_addi(GPIntReg &rd, int6 nzimm6) override;
    void c_addiw(GPIntReg &rd, int6 nzimm6) override;
    void c_addi16sp(GPIntReg &x2, int6 nzimm6) override;
    void c_addi4spn(GPIntReg &rdp, uint8 nzuimm8) override;
    void c_slli(GPIntReg &rd, uint6 nzuimm6) override;
    void c_srli(GPIntReg &rdp, uint6 nzuimm6) override;
    void c_srai(GPIntReg &rdp, uint6 nzuimm6) override;
    void c_andi(GPIntReg &rdp, int6 imm6) override;
    void c_mv(GPIntReg &rd, const GPIntReg &rs2) override;
    void c_add(GPIntReg &rd, const GPIntReg &rs2) override;
    void c_and(GPIntReg &rdp, const GPIntReg &rs2p) override;
    void c_or(GPIntReg &rdp, const GPIntReg &rs2p) override;
    void c_xor(GPIntReg &rdp, const GPIntReg &rs2p) override;
    void c_sub(GPIntReg &rdp, const GPIntReg &rs2p) override;
    void c_addw(GPIntReg &rdp, const GPIntReg &rs2p) override;
    void c_subw(GPIntReg &rdp, const GPIntReg &rs2p) override;

    ~Interpreter() override = default;

    void exec_instruction(const Instruction &in);

private:

    template<typename T, typename TOff = int12>
    void load_instruction_tmpl(GPIntReg &rd, const GPIntReg &rs, TOff offset) const;

    template<class T, class TOff = int12>
    void store_instruction_tmpl(const GPIntReg &rs, const GPIntReg &rs2, TOff offset);

    template<bool Rem = false, typename T>
    [[nodiscard]] static int64_t div_rem_tmpl(T lhs, T rhs);

    void handle_error(MemErr err) const;

    void handle_error(std::string_view msg) const;

    static uint64_t mul64x64_128high(uint64_t a, uint64_t b);

    [[nodiscard]] GPIntReg& x2_reg() const;

private:
    VM &m_vm;
    size_t m_current_line = SIZE_MAX;
};

}
