#pragma once
#include <common.hpp>
#include <Instruction.hpp>

namespace rv64 {
    class GPIntReg;
}

namespace rv64::is {
    class IBase {
    public:
        static constexpr int BASE_ID = 100;
        /// @brief add immediate.
        /// <br> rd = rs + imm12
        virtual void addi(GPIntReg &rd, const GPIntReg &rs, int12 imm12) = 0;

        /// @brief set less than immediate.
        /// <br> rd = signed(rs) < imm12 ? 1 : 0
        virtual void slti(GPIntReg &rd, const GPIntReg &rs, int12 imm12) = 0;

        /// @brief set less than immediate (unsigned).
        /// <br> rd = rs < imm ? 1 : 0
        virtual void sltiu(GPIntReg &rd, const GPIntReg &rs, int12 imm12) = 0;

        /// @brief logical AND immediate
        /// <br> rd = rs & imm12
        virtual void andi(GPIntReg &rd, const GPIntReg &rs, int12 imm12) = 0;

        /// @brief logical OR immediate
        /// <br> rd = rs | imm12
        virtual void ori(GPIntReg &rd, const GPIntReg &rs, int12 imm12) = 0;

        /// @brief logical XOR immediate
        /// <br> rd = rs ^ imm12
        virtual void xori(GPIntReg &rd, const GPIntReg &rs, int12 imm12) = 0;

        /// @brief shift left logical immediate
        /// <br> rd = rs << imm12
        virtual void slli(GPIntReg &rd, const GPIntReg &rs, uint6 imm12) = 0;

        /// @brief shift right logical immediate
        /// <br> rd = rs >> imm12
        virtual void srli(GPIntReg &rd, const GPIntReg &rs, uint6 uimm6) = 0;

        /// @brief shift right arithmetic immediate (the original sign bit is copied into the vacated upper bits).
        /// <br> rd = signed(rs) >> imm12
        virtual void srai(GPIntReg &rd, const GPIntReg &rs, uint6 uimm6) = 0;

        /// @brief load upper immediate
        /// <br> rd = sign_extend<i64>(imm20 << 12)
        virtual void lui(GPIntReg &rd, int20 imm20) = 0;

        /// @brief add upper immediate to pc
        /// <br> rd = pc + sign_extend<i64>(imm20 << 12)
        virtual void auipc(GPIntReg &rd, int20 imm20) = 0;

        /// @brief add two registers. (signed)
        /// <br> rd = rs1 + rs2
        virtual void add(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) = 0;

        /// @brief subtract two registers. (signed)
        /// <br> rd = rs1 - rs2
        virtual void sub(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) = 0;

        /// @brief set less than. (signed)
        /// <br> rd = rs1 < rs2 ? 1 : 0
        virtual void slt(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) = 0;

        /// @brief set less than. (unsigned)
        /// <br> rd = rs1 < rs2 ? 1 : 0
        virtual void sltu(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) = 0;

        /// @brief logical AND
        /// <br> rd = rs1 & rs2
        virtual void and_(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) = 0;

        /// @brief logical OR
        /// <br> rd = rs1 | rs2
        virtual void or_(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) = 0;

        /// @brief logical XOR
        /// <br> rd = rs1 ^ rs2
        virtual void xor_(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) = 0;

        /// @brief shift left logical
        /// <br> rd = rs1 << rs2[5:0] (the lower 6 bits of rs2)
        virtual void sll(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) = 0;

        /// @brief shift right logical
        /// <br> rd = rs1 >> rs2[5:0] (the lower 6 bits of rs2)
        virtual void srl(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) = 0;

        /// @brief shift right arithmetic (the original sign bit is copied into the vacated upper bits).
        /// <br> rd = signed(rs1) >> rs2[5:0] (the lower 6 bits of rs2)
        virtual void sra(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) = 0;

        /// @brief jump and link.
        /// <br> rd = pc + 4; goto pc + imm20
        virtual void jal(GPIntReg &rd, int20 imm20) = 0;

        /// @brief jump and link register.
        /// <br> rd = pc + 4; pc = rs + imm12
        virtual void jalr(GPIntReg &rd, const GPIntReg &rs, int12 imm12) = 0;

        /// @brief branch if equal.
        /// <br> pc += (rs1 == rs2) ? (imm12 << 1) : 4
        virtual void beq(const GPIntReg &rs1, const GPIntReg &rs2, int12 imm12) = 0;

        /// @brief branch if not equal.
        /// <br> pc += (rs1 != rs2) ? (imm12 << 1) : 4
        virtual void bne(const GPIntReg &rs1, const GPIntReg &rs2, int12 imm12) = 0;

        /// @brief branch if less than (signed).
        /// <br> pc += (rs1 < rs2) ? (imm12 << 1) : 4
        virtual void blt(const GPIntReg &rs1, const GPIntReg &rs2, int12 imm12) = 0;

        /// @brief branch if less than (unsigned).
        /// <br> pc += (rs1 < rs2) ? (imm12 << 1) : 4
        virtual void bltu(const GPIntReg &rs1, const GPIntReg &rs2, int12 imm12) = 0;

        /// @brief branch if greater than or equal (signed).
        /// <br> pc += (rs1 >= rs2) ? (imm12 << 1) : 4
        virtual void bge(const GPIntReg &rs1, const GPIntReg &rs2, int12 imm12) = 0;

        /// @brief branch if greater than or equal (unsigned).
        /// <br> pc += (rs1 >= rs2) ? (imm12 << 1) : 4
        virtual void bgeu(const GPIntReg &rs1, const GPIntReg &rs2, int12 imm12) = 0;

        /// @brief load 32-bit word from memory and sign-extend it to 64-bit.
        /// <br> rd = sign_extend<i64>(((i32*)mem)[rs + imm12])
        virtual void lw(GPIntReg &rd, const GPIntReg &rs, int12 imm12) = 0;

        /// @brief load 16-bit halfword from memory and sign-extend it to 64-bit.
        /// <br> rd = sign_extend<i64>(((i16*)mem)[rs + imm12])
        virtual void lh(GPIntReg &rd, const GPIntReg &rs, int12 imm12) = 0;

        /// @brief load 16-bit halfword from memory and zero-extend it to 64-bit.
        /// <br> rd = ((u16*)mem)[rs + imm12]
        virtual void lhu(GPIntReg &rd, const GPIntReg &rs, int12 imm12) = 0;

        /// @brief load 8-bit byte from memory and sign-extend it to 64-bit.
        /// <br> rd = sign_extend<i64>(((i8*)mem)[rs + imm12])
        virtual void lb(GPIntReg &rd, const GPIntReg &rs, int12 imm12) = 0;

        /// @brief load 8-bit byte from memory and zero-extend it to 64-bit.
        /// <br> rd = ((u8*)mem)[rs + imm12]
        virtual void lbu(GPIntReg &rd, const GPIntReg &rs, int12 imm12) = 0;

        /// @brief store 32-bit word to memory.
        /// <br> ((u32*)mem)[rs + imm12] = rs2[0:31] (the lower 32 bits of rs2)
        virtual void sw(const GPIntReg &rs, const GPIntReg &rs2, int12 imm12) = 0;

        /// @brief store 16-bit halfword to memory.
        /// <br> ((u16*)mem)[rs + imm12] = rs2[0:15] (the lower 16 bits of rs2)
        virtual void sh(const GPIntReg &rs, const GPIntReg &rs2, int12 imm12) = 0;

        /// @brief store 8-bit byte to memory.
        /// <br> ((u8*)mem)[rs + imm12] = rs2[0:7] (the lower 8 bits of rs2)
        virtual void sb(const GPIntReg &rs, const GPIntReg &rs2, int12 imm12) = 0;

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
        virtual void addiw(GPIntReg &rd, const GPIntReg &rs, int12 imm12) = 0;

        /// @brief logical left shift that operates on a 32-bit register and sign-extends the result to 64 bits.
        /// <br> rd = sign_extend<i64>(rs[31:0] << imm5)
        virtual void slliw(GPIntReg &rd, const GPIntReg &rs, uint5 imm5) = 0;

        /// @brief logical right shift that operates on a 32-bit register and sign-extends the result to 64 bits.
        /// <br> rd = sign_extend<i64>(rs[31:0] >> imm5)
        virtual void srliw(GPIntReg &rd, const GPIntReg &rs, uint5 imm5) = 0;

        /// @brief arithmetic right shift that operates on a 32-bit register and sign-extends the result to 64 bits.
        /// <br> rd = sign_extend<i64>(signed(rs[31:0]) >> imm5)
        virtual void sraiw(GPIntReg &rd, const GPIntReg &rs, uint5 imm5) = 0;

        /// @brief logical left shift that operates on a 32-bit register and sign-extends the result to 64 bits.
        /// <br> rd = sign_extend<i64>(rs1[31:0] << rs2[4:0])
        virtual void sllw(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) = 0;

        /// @brief logical right shift that operates on a 32-bit register and sign-extends the result to 64 bits.
        /// <br> rd = sign_extend<i64>(rs1[31:0] >> rs2[4:0])
        virtual void srlw(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) = 0;

        /// @brief arithmetic right shift that operates on a 32-bit register and sign-extends the result to 64 bits.
        /// <br> rd = sign_extend<i64>(signed(rs1[31:0]) >> rs2[4:0])
        virtual void sraw(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) = 0;

        /// @brief add 32-bit registers and sign-extend the result to 64 bits.
        /// <br> rd = sign_extend<i64>(rs1[31:0] + rs2[31:0])
        virtual void addw(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) = 0;

        /// @brief subtract 32-bit registers and sign-extend the result to 64 bits.
        /// <br> rd = sign_extend<i64>(rs1[31:0] - rs2[31:0])
        virtual void subw(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) = 0;

        /// @brief load 64-bit doubleword from memory.
        /// rd = ((i64*)mem)[rs + imm12]
        virtual void ld(GPIntReg &rd, const GPIntReg &rs, int12 imm12) = 0;

        /// @brief load 32-bit word from memory and zero-extend it to 64 bits.
        /// rd = ((u32*)mem)[rs + imm12]
        virtual void lwu(GPIntReg &rd, const GPIntReg &rs, int12 imm12) = 0;

        /// @brief store 64-bit doubleword to memory.
        /// <br> ((u64*)mem)[rs + imm12] = rs2
        virtual void sd(const GPIntReg &rs, const GPIntReg &rs2, int12 imm12) = 0;

        static constexpr std::array<InstProto, 51> list_inst() {
            const auto ireg = InstArgType::IntReg;
            const auto imm12 = InstArgType::Imm12;
            const auto uimm12 = InstArgType::UImm12;
            const auto uimm20 = InstArgType::UImm20;
            const auto imm20 = InstArgType::Imm20;
            return {
                {
                    {"addi"sv, {ireg, ireg, imm12}, (int) InstId::addi},
                    {"slt"sv, {ireg, ireg, ireg}, (int) InstId::slt},
                    {"slti"sv, {ireg, ireg, imm12}, (int) InstId::slti},
                    {"sltiu"sv, {ireg, ireg, uimm12}, (int) InstId::sltiu},
                    {"andi"sv, {ireg, ireg, imm12}, (int) InstId::andi},
                    {"ori"sv, {ireg, ireg, imm12}, (int) InstId::ori},
                    {"xori"sv, {ireg, ireg, imm12}, (int) InstId::xori},
                    {"slli"sv, {ireg, ireg, uimm12}, (int) InstId::slli},
                    {"srli"sv, {ireg, ireg, uimm12}, (int) InstId::srli},
                    {"srai"sv, {ireg, ireg, uimm12}, (int) InstId::srai},
                    {"lui"sv, {ireg, uimm20}, (int) InstId::lui},
                    {"auipc"sv, {ireg, uimm20}, (int) InstId::auipc},
                    {"add"sv, {ireg, ireg, ireg}, (int) InstId::add},
                    {"sub"sv, {ireg, ireg, ireg}, (int) InstId::sub},
                    {"and"sv, {ireg, ireg, ireg}, (int) InstId::and_},
                    {"or"sv, {ireg, ireg, ireg}, (int) InstId::or_},
                    {"xor"sv, {ireg, ireg, ireg}, (int) InstId::xor_},
                    {"sll"sv, {ireg, ireg, ireg}, (int) InstId::sll},
                    {"srl"sv, {ireg, ireg, ireg}, (int) InstId::srl},
                    {"sra"sv, {ireg, ireg, ireg}, (int) InstId::sra},
                    {"jal"sv, {ireg, imm20}, (int) InstId::jal},
                    {"jalr"sv, {ireg, ireg, imm12}, (int) InstId::jalr},
                    {"beq"sv, {ireg, ireg, imm12}, (int) InstId::beq},
                    {"bne"sv, {ireg, ireg, imm12}, (int) InstId::bne},
                    {"blt"sv, {ireg, ireg, imm12}, (int) InstId::blt},
                    {"bge"sv, {ireg, ireg, imm12}, (int) InstId::bge},
                    {"bltu"sv, {ireg, ireg, imm12}, (int) InstId::bltu},
                    {"bgeu"sv, {ireg, ireg, imm12}, (int) InstId::bgeu},
                    {"lw"sv, {ireg, ireg, imm12}, (int) InstId::lw},
                    {"lh"sv, {ireg, ireg, imm12}, (int) InstId::lh},
                    {"lhu"sv, {ireg, ireg, imm12}, (int) InstId::lhu},
                    {"lb"sv, {ireg, ireg, imm12}, (int) InstId::lb},
                    {"lbu"sv, {ireg, ireg, imm12}, (int) InstId::lbu},
                    {"sw"sv, {ireg, ireg, imm12}, (int) InstId::sw},
                    {"sh"sv, {ireg, ireg, imm12}, (int) InstId::sh},
                    {"sb"sv, {ireg, ireg, imm12}, (int) InstId::sb},
                    {"fence"sv, {}, (int) InstId::fence},
                    {"ecall"sv, {}, (int) InstId::ecall},
                    {"ebreak"sv, {}, (int) InstId::ebreak},
                    {"addiw"sv, {ireg, ireg, imm12}, (int) InstId::addiw},
                    {"slliw"sv, {ireg, ireg, imm12}, (int) InstId::slliw},
                    {"srliw"sv, {ireg, ireg, imm12}, (int) InstId::srliw},
                    {"sraiw"sv, {ireg, ireg, imm12}, (int) InstId::sraiw},
                    {"sllw"sv, {ireg, ireg, ireg}, (int) InstId::sllw},
                    {"srlw"sv, {ireg, ireg, ireg}, (int) InstId::srlw},
                    {"sraw"sv, {ireg, ireg, ireg}, (int) InstId::sraw},
                    {"addw"sv, {ireg, ireg, ireg}, (int) InstId::addw},
                    {"subw"sv, {ireg, ireg, ireg}, (int) InstId::subw},
                    {"ld"sv, {ireg, ireg, imm12}, (int) InstId::ld},
                    {"lwu"sv, {ireg, ireg, imm12}, (int) InstId::lwu},
                    {"sd"sv, {ireg, ireg, imm12}, (int) InstId::sd}
                }
            };
        }

        static_assert(BASE_ID > 0);

        enum class InstId : int {
            addi = BASE_ID,
            slt, slti, sltiu, andi, ori, xori, slli, srli, srai,
            lui, auipc, add, sub, and_, or_, xor_, sll, srl, sra,
            jal, jalr, beq, bne, blt, bge, bltu, bgeu, lw, lh, lhu,
            lb, lbu, sw, sh, sb, fence, ecall, ebreak, addiw, slliw,
            srliw, sraiw, sllw, srlw, sraw, addw, subw, ld, lwu, sd,
        };

        virtual ~IBase() = default;
    };
}
