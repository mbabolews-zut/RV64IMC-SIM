#pragma once
#include <common.hpp>
#include <Instruction.hpp>

namespace rv64 {
    class GPIntReg;
}

namespace rv64::is {
    class IExtensionC {
    public:
        static constexpr int IS_ID = 300; ///< Unique ID for the IExtensionC instruction set
        virtual ~IExtensionC() = default;

        virtual void c_lwsp(GPIntReg &rd, int6 imm6) = 0;
        virtual void c_ldsp(GPIntReg &rd, int6 imm6) = 0;
        virtual void c_fldsp(GPIntReg &rd, int6 imm6) = 0;
        virtual void c_swsp(GPIntReg &rd, int6 imm6) = 0;
        virtual void c_sdsp(GPIntReg &rd, int6 imm6) = 0;
        virtual void c_fsdsp(GPIntReg &rd, int6 imm6) = 0;
        virtual void c_lw(GPIntReg &rdp, const GPIntReg &rs1p, int5 imm5) = 0;
        virtual void c_ld(GPIntReg &rdp, const GPIntReg &rs1p, int5 imm5) = 0;
        virtual void c_fld(GPIntReg &rdp, const GPIntReg &rs1p, int5 imm5) = 0;
        virtual void c_sw(const GPIntReg &rs2p, const GPIntReg &rs1p, int5 imm5) = 0;
        virtual void c_sd(const GPIntReg &rs2p, const GPIntReg &rs1p, int5 imm5) = 0;
        virtual void c_fsd(const GPIntReg &rs2p, const GPIntReg &rs1p, int5 imm5) = 0;
        virtual void c_j(int11 imm11) = 0;
        virtual void c_jr(const GPIntReg &rs1) = 0;
        virtual void c_jalr(const GPIntReg &rs1) = 0;
        virtual void c_beqz(const GPIntReg &rs1p, int8 imm8) = 0;
        virtual void c_bnez(const GPIntReg &rs1p, int8 imm8) = 0;
        virtual void c_li(const GPIntReg &rd, int6 imm6) = 0;
        virtual void c_lui(const GPIntReg &rd, int6 nzimm6) = 0;
        virtual void c_addi(const GPIntReg &rd, int6 nzimm6) = 0;
        virtual void c_addiw(const GPIntReg &rd, int6 nzimm6) = 0;
        virtual void c_addi16sp(const GPIntReg &x2, int6 nzimm6) = 0;
        virtual void c_addi4spn(const GPIntReg &rdp, uint8 nzuimm8) = 0;
        virtual void c_slli(const GPIntReg &rd, uint6 nzuimm6) = 0;
        virtual void c_srli(const GPIntReg &rdp, uint6 nzuimm6) = 0;
        virtual void c_srai(const GPIntReg &rdp, uint6 uimm6) = 0;
        virtual void c_andi(const GPIntReg &rdp, int6 imm6) = 0;
        virtual void c_mv(const GPIntReg &rd, const GPIntReg &rs2) = 0;
        virtual void c_add(const GPIntReg &rd, const GPIntReg &rs2) = 0;
        virtual void c_and(const GPIntReg &rdp, const GPIntReg &rs2p) = 0;
        virtual void c_or(const GPIntReg &rdp, const GPIntReg &rs2p) = 0;
        virtual void c_xor(const GPIntReg &rdp, const GPIntReg &rs2p) = 0;
        virtual void c_sub(const GPIntReg &rdp, const GPIntReg &rs2p) = 0;
        virtual void c_addw(const GPIntReg &rdp, const GPIntReg &rs2p) = 0;
        virtual void c_subw(const GPIntReg &rdp, const GPIntReg &rs2p) = 0;

        static constexpr std::array<InstProto, 35> list_inst() {
            return {{
                {"c.lwsp"sv, {InstArgType::IntReg, InstArgType::Imm6}, (int)InstId::c_lwsp},
                {"c.ldsp"sv, {InstArgType::IntReg, InstArgType::Imm6}, (int)InstId::c_ldsp},
                {"c.fldsp"sv, {InstArgType::IntReg, InstArgType::Imm6}, (int)InstId::c_fldsp},
                {"c.swsp"sv, {InstArgType::IntReg, InstArgType::Imm6}, (int)InstId::c_swsp},
                {"c.sdsp"sv, {InstArgType::IntReg, InstArgType::Imm6}, (int)InstId::c_sdsp },
                {"c.fsdsp"sv, {InstArgType::IntReg, InstArgType::Imm6}, (int)InstId::c_fsdsp},
                {"c.lw"sv, {InstArgType::IntRegP, InstArgType::IntRegP, InstArgType::Imm5}, (int)InstId::c_lw},
                {"c.ld"sv, {InstArgType::IntRegP, InstArgType::IntRegP, InstArgType::Imm5}, (int)InstId::c_ld },
                {"c.fld"sv, {InstArgType::IntRegP, InstArgType::IntRegP, InstArgType::Imm5}, (int)InstId::c_fld},
                {"c.sw"sv, {InstArgType::IntRegP, InstArgType::IntRegP, InstArgType::Imm5}, (int)InstId::c_sw},
                {"c.sd"sv, {InstArgType::IntRegP, InstArgType::IntRegP, InstArgType::Imm5}, (int)InstId::c_sd},
                {"c.fsd"sv, {InstArgType::IntRegP, InstArgType::IntRegP, InstArgType::Imm5}, (int)InstId::c_fsd},
                {"c.j"sv, {InstArgType::Imm11}, (int)InstId::c_j},
                {"c.jr"sv, {InstArgType::IntReg}, (int)InstId::c_jr},
                {"c.jalr"sv, {InstArgType::IntReg}, (int)InstId::c_jalr},
                {"c.beqz"sv, {InstArgType::IntRegP, InstArgType::Imm8}, (int)InstId::c_beqz},
                {"c.bnez"sv, {InstArgType::IntRegP, InstArgType::Imm8}, (int)InstId::c_bnez},
                {"c.li"sv, {InstArgType::IntReg, InstArgType::Imm6}, (int)InstId::c_li},
                {"c.lui"sv, {InstArgType::IntReg, InstArgType::Imm6}, (int)InstId::c_lui},
                {"c.addi"sv, {InstArgType::IntReg, InstArgType::Imm6}, (int)InstId::c_addi},
                {"c.addiw"sv, {InstArgType::IntReg, InstArgType::Imm6}, (int)InstId::c_addiw},
                {"c.addi16sp"sv, {InstArgType::IntReg, InstArgType::Imm6}, (int)InstId::c_addi16sp},
                {"c.addi4spn"sv, {InstArgType::IntRegP, InstArgType::UImm8}, (int)InstId::c_addi4spn},
                {"c.slli"sv, {InstArgType::IntReg, InstArgType::UImm6}, (int)InstId::c_slli},
                {"c.srli"sv, {InstArgType::IntRegP, InstArgType::UImm6}, (int)InstId::c_srli},
                {"c.srai"sv, {InstArgType::IntRegP, InstArgType::UImm6}, (int)InstId::c_srai},
                {"c.andi"sv, {InstArgType::IntRegP, InstArgType::Imm6}, (int)InstId::c_andi},
                {"c.mv"sv, {InstArgType::IntReg, InstArgType::IntReg}, (int)InstId::c_mv},
                {"c.add"sv, {InstArgType::IntReg, InstArgType::IntReg}, (int)InstId::c_add},
                {"c.and"sv, {InstArgType::IntRegP, InstArgType::IntRegP}, (int)InstId::c_and},
                {"c.or"sv, {InstArgType::IntRegP, InstArgType::IntRegP}, (int)InstId::c_or},
                {"c.xor"sv, {InstArgType::IntRegP, InstArgType::IntRegP}, (int)InstId::c_xor},
                {"c.sub"sv, {InstArgType::IntRegP, InstArgType::IntRegP}, (int)InstId::c_sub},
                {"c.addw"sv, {InstArgType::IntRegP, InstArgType::IntRegP}, (int)InstId::c_addw},
                {"c.subw"sv, {InstArgType::IntRegP, InstArgType::IntRegP}, (int)InstId::c_subw},
            }};
        }

        static_assert(IS_ID > 0);
        enum class InstId {
            c_lwsp = IS_ID + 1, c_ldsp, c_fldsp, c_swsp, c_sdsp, c_fsdsp,
            c_lw, c_ld, c_fld, c_sw, c_sd, c_fsd,
            c_j, c_jr, c_jalr, c_beqz, c_bnez,
            c_li, c_lui, c_addi, c_addiw, c_addi16sp, c_addi4spn,
            c_slli, c_srli, c_srai, c_andi,
            c_mv, c_add, c_and, c_or, c_xor, c_sub, c_addw, c_subw
        };
    };

}