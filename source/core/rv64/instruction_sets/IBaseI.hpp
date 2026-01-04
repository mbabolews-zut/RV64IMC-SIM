#pragma once
#include <common.hpp>
#include <Instruction.hpp>

namespace rv64 {
    class GPIntReg;
}

namespace rv64::is {
    class IBaseI {
    public:
        static constexpr int IS_ID = 100; ///< Unique ID for the IBaseI instruction set

        virtual void addi(GPIntReg &rd, const GPIntReg &rs, int12 imm12) = 0;
        virtual void slti(GPIntReg &rd, const GPIntReg &rs, int12 imm12) = 0;
        virtual void sltiu(GPIntReg &rd, const GPIntReg &rs, int12 imm12) = 0;
        virtual void andi(GPIntReg &rd, const GPIntReg &rs, int12 imm12) = 0;
        virtual void ori(GPIntReg &rd, const GPIntReg &rs, int12 imm12) = 0;
        virtual void xori(GPIntReg &rd, const GPIntReg &rs, int12 imm12) = 0;
        virtual void slli(GPIntReg &rd, const GPIntReg &rs, uint6 imm12) = 0;
        virtual void srli(GPIntReg &rd, const GPIntReg &rs, uint6 uimm6) = 0;
        virtual void srai(GPIntReg &rd, const GPIntReg &rs, uint6 uimm6) = 0;
        virtual void lui(GPIntReg &rd, int20 imm20) = 0;
        virtual void auipc(GPIntReg &rd, int20 imm20) = 0;
        virtual void add(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) = 0;
        virtual void sub(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) = 0;
        virtual void slt(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) = 0;
        virtual void sltu(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) = 0;
        virtual void and_(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) = 0;
        virtual void or_(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) = 0;
        virtual void xor_(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) = 0;
        virtual void sll(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) = 0;
        virtual void srl(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) = 0;
        virtual void sra(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) = 0;
        virtual void jal(GPIntReg &rd, int20 imm20) = 0;
        virtual void jalr(GPIntReg &rd, const GPIntReg &rs, int12 imm12) = 0;
        virtual void beq(const GPIntReg &rs1, const GPIntReg &rs2, int12 imm12) = 0;
        virtual void bne(const GPIntReg &rs1, const GPIntReg &rs2, int12 imm12) = 0;
        virtual void blt(const GPIntReg &rs1, const GPIntReg &rs2, int12 imm12) = 0;
        virtual void bltu(const GPIntReg &rs1, const GPIntReg &rs2, int12 imm12) = 0;
        virtual void bge(const GPIntReg &rs1, const GPIntReg &rs2, int12 imm12) = 0;
        virtual void bgeu(const GPIntReg &rs1, const GPIntReg &rs2, int12 imm12) = 0;
        virtual void lw(GPIntReg &rd, const GPIntReg &rs, int12 imm12) = 0;
        virtual void lh(GPIntReg &rd, const GPIntReg &rs, int12 imm12) = 0;
        virtual void lhu(GPIntReg &rd, const GPIntReg &rs, int12 imm12) = 0;
        virtual void lb(GPIntReg &rd, const GPIntReg &rs, int12 imm12) = 0;
        virtual void lbu(GPIntReg &rd, const GPIntReg &rs, int12 imm12) = 0;
        virtual void sw(const GPIntReg &rs2, const GPIntReg &rs1, int12 imm12) = 0;
        virtual void sh(const GPIntReg &rs2, const GPIntReg &rs1, int12 imm12) = 0;
        virtual void sb(const GPIntReg &rs2, const GPIntReg &rs1, int12 imm12) = 0;
        virtual void fence() = 0;
        virtual void ecall() = 0;
        virtual void ebreak() = 0;
        virtual void addiw(GPIntReg &rd, const GPIntReg &rs, int12 imm12) = 0;
        virtual void slliw(GPIntReg &rd, const GPIntReg &rs, uint5 imm5) = 0;
        virtual void srliw(GPIntReg &rd, const GPIntReg &rs, uint5 imm5) = 0;
        virtual void sraiw(GPIntReg &rd, const GPIntReg &rs, uint5 imm5) = 0;
        virtual void sllw(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) = 0;
        virtual void srlw(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) = 0;
        virtual void sraw(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) = 0;
        virtual void addw(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) = 0;
        virtual void subw(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) = 0;
        virtual void ld(GPIntReg &rd, const GPIntReg &rs, int12 imm12) = 0;
        virtual void lwu(GPIntReg &rd, const GPIntReg &rs, int12 imm12) = 0;
        virtual void sd(const GPIntReg &rs2, const GPIntReg &rs1, int12 imm12) = 0;
        virtual void nop() = 0;

        static constexpr std::array<InstProto, 53> list_inst() {
            const auto ireg = InstArgType::IntReg;
            const auto imm12 = InstArgType::Imm12;
            const auto uimm5 = InstArgType::UImm5;
            const auto uimm6 = InstArgType::UImm6;
            const auto uimm12 = InstArgType::UImm12;
            const auto imm20 = InstArgType::Imm20;
            return {
                {
                    {"addi"sv, {ireg, ireg, imm12}, (int) InstId::addi},
                    {"slt"sv, {ireg, ireg, ireg}, (int) InstId::slt},
                    {"sltu"sv, {ireg, ireg, ireg}, (int) InstId::sltu},
                    {"slti"sv, {ireg, ireg, imm12}, (int) InstId::slti},
                    {"sltiu"sv, {ireg, ireg, uimm12}, (int) InstId::sltiu},
                    {"andi"sv, {ireg, ireg, imm12}, (int) InstId::andi},
                    {"ori"sv, {ireg, ireg, imm12}, (int) InstId::ori},
                    {"xori"sv, {ireg, ireg, imm12}, (int) InstId::xori},
                    {"slli"sv, {ireg, ireg, uimm6}, (int) InstId::slli},
                    {"srli"sv, {ireg, ireg, uimm6}, (int) InstId::srli},
                    {"srai"sv, {ireg, ireg, uimm6}, (int) InstId::srai},
                    {"lui"sv, {ireg, imm20}, (int) InstId::lui},
                    {"auipc"sv, {ireg, imm20}, (int) InstId::auipc},
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
                    {"slliw"sv, {ireg, ireg, uimm5}, (int) InstId::slliw},
                    {"srliw"sv, {ireg, ireg, uimm5}, (int) InstId::srliw},
                    {"sraiw"sv, {ireg, ireg, uimm5}, (int) InstId::sraiw},
                    {"sllw"sv, {ireg, ireg, ireg}, (int) InstId::sllw},
                    {"srlw"sv, {ireg, ireg, ireg}, (int) InstId::srlw},
                    {"sraw"sv, {ireg, ireg, ireg}, (int) InstId::sraw},
                    {"addw"sv, {ireg, ireg, ireg}, (int) InstId::addw},
                    {"subw"sv, {ireg, ireg, ireg}, (int) InstId::subw},
                    {"ld"sv, {ireg, ireg, imm12}, (int) InstId::ld},
                    {"lwu"sv, {ireg, ireg, imm12}, (int) InstId::lwu},
                    {"sd"sv, {ireg, ireg, imm12}, (int) InstId::sd},
                    {"nop"sv, {}, (int) InstId::nop}
                }
            };
        }

        static_assert(IS_ID > 0);

        enum class InstId : int {
            addi = IS_ID,
            slt, sltu, slti, sltiu, andi, ori, xori, slli, srli, srai,
            lui, auipc, add, sub, and_, or_, xor_, sll, srl, sra,
            jal, jalr, beq, bne, blt, bge, bltu, bgeu, lw, lh, lhu,
            lb, lbu, sw, sh, sb, fence, ecall, ebreak, addiw, slliw,
            srliw, sraiw, sllw, srlw, sraw, addw, subw, ld, lwu, sd, nop
        };

        virtual ~IBaseI() = default;
    };
}
