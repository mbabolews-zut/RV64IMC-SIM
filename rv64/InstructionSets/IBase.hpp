#pragma once
#include <common.hpp>

namespace rv64 {
    class IntReg;
}

namespace rv64::is {

    class IBase {
    public:
        /// @brief add immediate.
        /// <br> rd = rs + imm12
        virtual void addi(IntReg &rd, const IntReg &rs, int12 imm12) = 0;

        /// @brief set less than immediate.
        /// <br> rd = signed(rs) < imm12 ? 1 : 0
        virtual void slti(IntReg &rd, const IntReg &rs, int12 imm12) = 0;

        /// @brief set less than immediate (unsigned).
        /// <br> rd = rs < imm ? 1 : 0
        virtual void sltiu(IntReg &rd, const IntReg &rs, uint12 imm12) = 0;

        /// @brief logical AND immediate
        /// <br> rd = rs & imm12
        virtual void andi(IntReg &rd, const IntReg &rs, int12 imm12) = 0;

        /// @brief logical OR immediate
        /// <br> rd = rs | imm12
        virtual void ori(IntReg &rd, const IntReg &rs, int12 imm12) = 0;

        /// @brief logical XOR immediate
        /// <br> rd = rs ^ imm12
        virtual void xori(IntReg &rd, const IntReg &rs, uint12 imm12) = 0;

        /// @brief shift left logical immediate
        /// <br> rd = rs << imm12
        virtual void slli(IntReg &rd, const IntReg &rs, uint12 imm12) = 0;

        /// @brief shift right logical immediate
        /// <br> rd = rs >> imm12
        virtual void srli(IntReg &rd, const IntReg &rs, uint12 imm12) = 0;

        /// @brief shift right arithmetic immediate (the original sign bit is copied into the vacated upper bits).
        /// <br> rd = signed(rs) >> imm12
        virtual void srai(IntReg &rd, const IntReg &rs, uint12 imm12) = 0;

        /// @brief load upper immediate
        /// <br> rd = sign_extend<i64>(imm20 << 12)
        virtual void lui(IntReg &rd, int20 imm20) = 0;

        /// @brief add upper immediate to pc
        /// <br> rd = pc + sign_extend<i64>(imm20 << 12)
        virtual void auipc(IntReg &rd, int20 imm20) = 0;

        /// @brief add two registers. (signed)
        /// <br> rd = rs1 + rs2
        virtual void add(IntReg &rd, const IntReg &rs1, const IntReg &rs2) = 0;

        /// @brief subtract two registers. (signed)
        /// <br> rd = rs1 - rs2
        virtual void sub(IntReg &rd, const IntReg &rs1, const IntReg &rs2) = 0;

        /// @brief set less than. (signed)
        /// <br> rd = rs1 < rs2 ? 1 : 0
        virtual void slt(IntReg &rd, const IntReg &rs1, const IntReg &rs2) = 0;

        /// @brief set less than. (unsigned)
        /// <br> rd = rs1 < rs2 ? 1 : 0
        virtual void sltu(IntReg &rd, const IntReg &rs1, const IntReg &rs2) = 0;

        /// @brief logical AND
        /// <br> rd = rs1 & rs2
        virtual void and_(IntReg &rd, const IntReg &rs1, const IntReg &rs2) = 0;

        /// @brief logical OR
        /// <br> rd = rs1 | rs2
        virtual void or_(IntReg &rd, const IntReg &rs1, const IntReg &rs2) = 0;

        /// @brief logical XOR
        /// <br> rd = rs1 ^ rs2
        virtual void xor_(IntReg &rd, const IntReg &rs1, const IntReg &rs2) = 0;

        /// @brief shift left logical
        /// <br> rd = rs1 << rs2[5:0] (the lower 6 bits of rs2)
        virtual void sll(IntReg &rd, const IntReg &rs1, const IntReg &rs2) = 0;

        /// @brief shift right logical
        /// <br> rd = rs1 >> rs2[5:0] (the lower 6 bits of rs2)
        virtual void srl(IntReg &rd, const IntReg &rs1, const IntReg &rs2) = 0;

        /// @brief shift right arithmetic (the original sign bit is copied into the vacated upper bits).
        /// <br> rd = signed(rs1) >> rs2[5:0] (the lower 6 bits of rs2)
        virtual void sra(IntReg &rd, const IntReg &rs1, const IntReg &rs2) = 0;

        /// @brief jump and link.
        /// <br> rd = pc + 4; goto pc + imm20
        virtual void jal(IntReg &rd, int imm20) = 0;

        /// @brief jump and link register.
        /// <br> rd = pc + 4; pc = rs + imm12
        virtual void jalr(IntReg &rd, const IntReg &rs, int12 imm12) = 0;

        /// @brief branch if equal.
        /// <br> pc += (rs1 == rs2) ? (imm12 << 1) : 4
        virtual void beq(const IntReg &rs1, const IntReg &rs2, int imm12) = 0;

        /// @brief branch if not equal.
        /// <br> pc += (rs1 != rs2) ? (imm12 << 1) : 4
        virtual void bne(const IntReg &rs1, const IntReg &rs2, int imm12) = 0;

        /// @brief branch if less than (signed).
        /// <br> pc += (rs1 < rs2) ? (imm12 << 1) : 4
        virtual void blt(const IntReg &rs1, const IntReg &rs2, int12 imm12) = 0;

        /// @brief branch if less than (unsigned).
        /// <br> pc += (rs1 < rs2) ? (imm12 << 1) : 4
        virtual void bltu(const IntReg &rs1, const IntReg &rs2, int12 imm12) = 0;

        /// @brief branch if greater than or equal (signed).
        /// <br> pc += (rs1 >= rs2) ? (imm12 << 1) : 4
        virtual void bge(const IntReg &rs1, const IntReg &rs2, int12 imm12) = 0;

        /// @brief branch if greater than or equal (unsigned).
        /// <br> pc += (rs1 >= rs2) ? (imm12 << 1) : 4
        virtual void bgeu(const IntReg &rs1, const IntReg &rs2, int12 imm12) = 0;

        /// @brief load 32-bit word from memory and sign-extend it to 64-bit.
        /// <br> rd = sign_extend<i64>(((i32*)mem)[rs + imm12])
        virtual void lw(IntReg &rd, const IntReg &rs, int12 imm12) = 0;

        /// @brief load 16-bit halfword from memory and sign-extend it to 64-bit.
        /// <br> rd = sign_extend<i64>(((i16*)mem)[rs + imm12])
        virtual void lh(IntReg &rd, const IntReg &rs, int12 imm12) = 0;

        /// @brief load 16-bit halfword from memory and zero-extend it to 64-bit.
        /// <br> rd = ((u16*)mem)[rs + imm12]
        virtual void lhu(IntReg &rd, const IntReg &rs, int12 imm12) = 0;

        /// @brief load 8-bit byte from memory and sign-extend it to 64-bit.
        /// <br> rd = sign_extend<i64>(((i8*)mem)[rs + imm12])
        virtual void lb(IntReg &rd, const IntReg &rs, int12 imm12) = 0;

        /// @brief load 8-bit byte from memory and zero-extend it to 64-bit.
        /// <br> rd = ((u8*)mem)[rs + imm12]
        virtual void lbu(IntReg &rd, const IntReg &rs, int12 imm12) = 0;

        /// @brief store 32-bit word to memory.
        /// <br> ((u32*)mem)[rs + imm12] = rs2[0:31] (the lower 32 bits of rs2)
        virtual void sw(const IntReg &rs, const IntReg &rs2, int12 imm12) = 0;

        /// @brief store 16-bit halfword to memory.
        /// <br> ((u16*)mem)[rs + imm12] = rs2[0:15] (the lower 16 bits of rs2)
        virtual void sh(const IntReg &rs, const IntReg &rs2, int12 imm12) = 0;

        /// @brief store 8-bit byte to memory.
        /// <br> ((u8*)mem)[rs + imm12] = rs2[0:7] (the lower 8 bits of rs2)
        virtual void sb(const IntReg &rs, const IntReg &rs2, int12 imm12) = 0;

        /// @brief fence instruction. https://en.wikipedia.org/wiki/Memory_barrier
        /// <br> does nothing on single-threaded systems.
        virtual void fence() = 0;

        /// @brief used for system calls such as printing to the console.
        virtual void ecall() = 0;

        /// @brief breakpoint instruction used for debugging.
        virtual void ebreak() = 0;

        //
        // rv64 & rv128 only instructions
        //

        /// @brief adds imm12 to 32-bit register and sign-extends the result to 64 bits.
        /// <br> rd = sign_extend<i64>(rs[31:0] + signed(imm12))
        virtual void addiw(IntReg &rd, const IntReg &rs, int12 imm12) = 0;

        /// @brief logical left shift that operates on a 32-bit register and sign-extends the result to 64 bits.
        /// <br> rd = sign_extend<i64>(rs[31:0] << imm5)
        virtual void slliw(IntReg &rd, const IntReg &rs, uint5 imm5) = 0;

        /// @brief logical right shift that operates on a 32-bit register and sign-extends the result to 64 bits.
        /// <br> rd = sign_extend<i64>(rs[31:0] >> imm5)
        virtual void srliw(IntReg &rd, const IntReg &rs, uint5 imm5) = 0;

        /// @brief arithmetic right shift that operates on a 32-bit register and sign-extends the result to 64 bits.
        /// <br> rd = sign_extend<i64>(signed(rs[31:0]) >> imm5)
        virtual void sraiw(IntReg &rd, const IntReg &rs, uint5 imm5) = 0;

        /// @brief logical left shift that operates on a 32-bit register and sign-extends the result to 64 bits.
        /// <br> rd = sign_extend<i64>(rs1[31:0] << rs2[4:0])
        virtual void sllw(IntReg &rd, const IntReg &rs1, const IntReg &rs2) = 0;

        /// @brief logical right shift that operates on a 32-bit register and sign-extends the result to 64 bits.
        /// <br> rd = sign_extend<i64>(rs1[31:0] >> rs2[4:0])
        virtual void srlw(IntReg &rd, const IntReg &rs1, const IntReg &rs2) = 0;

        /// @brief arithmetic right shift that operates on a 32-bit register and sign-extends the result to 64 bits.
        /// <br> rd = sign_extend<i64>(signed(rs1[31:0]) >> rs2[4:0])
        virtual void sraw(IntReg &rd, const IntReg &rs1, const IntReg &rs2) = 0;

        /// @brief add 32-bit registers and sign-extend the result to 64 bits.
        /// <br> rd = sign_extend<i64>(rs1[31:0] + rs2[31:0])
        virtual void addw(IntReg &rd, const IntReg &rs1, const IntReg &rs2) = 0;

        /// @brief subtract 32-bit registers and sign-extend the result to 64 bits.
        /// <br> rd = sign_extend<i64>(rs1[31:0] - rs2[31:0])
        virtual void subw(IntReg &rd, const IntReg &rs1, const IntReg &rs2) = 0;

        /// @brief load 64-bit doubleword from memory.
        /// rd = ((i64*)mem)[rs + imm12]
        virtual void ld(IntReg &rd, const IntReg &rs, int12 imm12) = 0;

        /// @brief load 32-bit word from memory and zero-extend it to 64 bits.
        /// rd = ((u32*)mem)[rs + imm12]
        virtual void lwu(IntReg &rd, const IntReg &rs, int12 imm12) = 0;

        /// @brief store 64-bit doubleword to memory.
        /// <br> ((u64*)mem)[rs + imm12] = rs2
        virtual void sd(const IntReg &rs, const IntReg &rs2, int12 imm12) = 0;


        virtual ~IBase() = default;
    };
}
