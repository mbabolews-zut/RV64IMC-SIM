#include "Interpreter.hpp"

#include <any>
#include <cassert>
#include <format>
#include <rv64/Cpu.hpp>
#include <ui.hpp>

#include "VM.hpp"

#if defined(_MSC_VER) && defined(_M_IX64)
#   include <intrin.h>
#   pragma intrinsic (__mulh)
#   pragma intrinsic (__umulh)
#endif


namespace rv64 {
    Interpreter &Interpreter::operator=(Interpreter &&other) {
        m_current_line = other.m_current_line;
        return *this;
    }

    void Interpreter::addi(GPIntReg &rd, const GPIntReg &rs, int12 imm12) {
        rd = rs.sval() + imm12;
    }

    void Interpreter::slti(GPIntReg &rd, const GPIntReg &rs, int12 imm12) {
        rd = rs.sval() < imm12 ? 1 : 0;
    }

    void Interpreter::sltiu(GPIntReg &rd, const GPIntReg &rs, int12 imm12) {
        rd = rs.val() < std::bit_cast<uint64_t>((int64_t) imm12) ? 1 : 0;
    }

    void Interpreter::andi(GPIntReg &rd, const GPIntReg &rs, int12 imm12) {
        rd = rs.sval() & imm12;
    }

    void Interpreter::ori(GPIntReg &rd, const GPIntReg &rs, int12 imm12) {
        rd = rs.sval() | imm12;
    }

    void Interpreter::xori(GPIntReg &rd, const GPIntReg &rs, int12 imm12) {
        rd = rs.sval() ^ imm12;
    }

    void Interpreter::slli(GPIntReg &rd, const GPIntReg &rs, uint6 uimm6) {
        rd = rs.val() << uimm6;
    }

    void Interpreter::srli(GPIntReg &rd, const GPIntReg &rs, uint6 uimm6) {
        rd = rs.val() >> uimm6;
    }

    void Interpreter::srai(GPIntReg &rd, const GPIntReg &rs, uint6 uimm6) {
        rd = rs.sval() >> uimm6; // C++ 20 defined behaviour
    }

    void Interpreter::lui(GPIntReg &rd, int20 imm20) {
        rd = static_cast<int32_t>(imm20 << 12);
    }

    void Interpreter::auipc(GPIntReg &rd, int20 imm20) {
        rd = m_vm.m_cpu.get_pc() + (imm20 << 12);
    }

    void Interpreter::add(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) {
        rd = rs1.sval() + rs2.sval();
    }

    void Interpreter::sub(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) {
        rd = rs1.sval() - rs2.sval();
    }

    void Interpreter::slt(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) {
        rd = rs1.sval() < rs2.sval() ? 1 : 0;
    }

    void Interpreter::sltu(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) {
        rd = rs1.val() < rs2.val() ? 1 : 0;
    }

    void Interpreter::and_(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) {
        rd = rs1.val() & rs2.val();
    }

    void Interpreter::or_(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) {
        rd = rs1.val() | rs2.val();
    }

    void Interpreter::xor_(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) {
        rd = rs1.val() ^ rs2.val();
    }

    void Interpreter::sll(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) {
        rd = rs1.val() << (rs2.val() & 0x3F);
    }

    void Interpreter::srl(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) {
        rd = rs1.val() >> (rs2.val() & 0x3F);
    }

    void Interpreter::sra(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) {
        rd = rs1.sval() >> (rs2.val() & 0x3F); // C++ 20 defined behaviour
    }

    void Interpreter::jal(GPIntReg &rd, int20 imm20) {
        rd = m_vm.m_cpu.get_pc();
        // Offset is relative to current_pc, but PC already advanced by 4
        m_vm.m_cpu.move_pc(imm20 * 2 - 4);
    }

    void Interpreter::jalr(GPIntReg &rd, const GPIntReg &rs, int12 imm12) {
        rd = m_vm.m_cpu.get_pc();
        m_vm.m_cpu.set_pc(rs.sval() + imm12);
    }

    void Interpreter::beq(const GPIntReg &rs1, const GPIntReg &rs2, int12 imm12) {
        if (rs1.sval() == rs2.sval()) {
            // Offset is relative to current_pc, but PC already advanced by 4
            m_vm.m_cpu.move_pc(imm12 * 2 - 4);
        }
    }

    void Interpreter::bne(const GPIntReg &rs1, const GPIntReg &rs2, int12 imm12) {
        if (rs1.sval() != rs2.sval()) {
            m_vm.m_cpu.move_pc(imm12 * 2 - 4);
        }
    }

    void Interpreter::blt(const GPIntReg &rs1, const GPIntReg &rs2, int12 imm12) {
        if (rs1.sval() < rs2.sval()) {
            m_vm.m_cpu.move_pc(imm12 * 2 - 4);
        }
    }

    void Interpreter::bltu(const GPIntReg &rs1, const GPIntReg &rs2, int12 imm12) {
        if (rs1.val() < rs2.val()) {
            m_vm.m_cpu.move_pc(imm12 * 2 - 4);
        }
    }

    void Interpreter::bge(const GPIntReg &rs1, const GPIntReg &rs2, int12 imm12) {
        if (rs1.sval() >= rs2.sval()) {
            m_vm.m_cpu.move_pc(imm12 * 2 - 4);
        }
    }

    void Interpreter::bgeu(const GPIntReg &rs1, const GPIntReg &rs2, int12 imm12) {
        if (rs1.val() >= rs2.val()) {
            m_vm.m_cpu.move_pc(imm12 * 2 - 4);
        }
    }

    void Interpreter::lw(GPIntReg &rd, const GPIntReg &rs, int12 imm12) {
        load_instruction_tmpl<int32_t>(rd, rs, imm12);
    }

    void Interpreter::lh(GPIntReg &rd, const GPIntReg &rs, int12 imm12) {
        load_instruction_tmpl<int16_t>(rd, rs, imm12);
    }

    void Interpreter::lhu(GPIntReg &rd, const GPIntReg &rs, int12 imm12) {
        load_instruction_tmpl<uint16_t>(rd, rs, imm12);
    }

    void Interpreter::lb(GPIntReg &rd, const GPIntReg &rs, int12 imm12) {
        load_instruction_tmpl<int8_t>(rd, rs, imm12);
    }

    void Interpreter::lbu(GPIntReg &rd, const GPIntReg &rs, int12 imm12) {
        load_instruction_tmpl<uint8_t>(rd, rs, imm12);
    }

    void Interpreter::sw(const GPIntReg &rs2, const GPIntReg &rs1, int12 imm12) {
        store_instruction_tmpl<uint32_t>(rs1, rs2, imm12);
    }

    void Interpreter::sh(const GPIntReg &rs2, const GPIntReg &rs1, int12 imm12) {
        store_instruction_tmpl<uint16_t>(rs1, rs2, imm12);
    }

    void Interpreter::sb(const GPIntReg &rs2, const GPIntReg &rs1, int12 imm12) {
        store_instruction_tmpl<uint8_t>(rs1, rs2, imm12);
    }

    void Interpreter::fence() {
        // Implementation is not needed for single threaded systems.
    }

    void Interpreter::ecall() {
        auto &a0 = m_vm.m_cpu.reg(10);
        const auto &a1 = m_vm.m_cpu.reg(11);
        MemErr err{};

        switch (a0.sval()) {
            case 1:
                ui::print_output(std::to_string(a1.sval()));
                return;
            case 4:
                ui::print_output(m_vm.m_memory.load_string(a1.val(), err));
                break;
            case 9:
                a0 = m_vm.m_memory.sbrk(a1.sval(), err);
                break;
            case 10:
                m_vm.terminate(0);
                return;
            case 11:
                ui::print_output(std::string(1, static_cast<char>(a1.val() & 0xFF)));
                return;
            case 17:
                m_vm.terminate(static_cast<int>(a1.sval()));
                return;
            case -2137: // Secret exit code to suppress clangd "(err != MemErr::None) is always false",
                // err can be modified in the cases above.
                m_vm.terminate(-2137);
                err = MemErr::ProgramExit;
                break;
            default:
                ui::print_warning(std::format("Unsupported ecall code: {}", a0.sval()));
                return;
        }
        if (err != MemErr::None) handle_error(err);
    }

    void Interpreter::ebreak() {
        m_vm.breakpoint_hit();
    }

    void Interpreter::addiw(GPIntReg &rd, const GPIntReg &rs, int12 imm12) {
        rd = rs.as_i32() + static_cast<int32_t>(imm12);
    }

    void Interpreter::slliw(GPIntReg &rd, const GPIntReg &rs, uint5 imm5) {
        rd = rs.as_i32() << imm5;
    }

    void Interpreter::srliw(GPIntReg &rd, const GPIntReg &rs, uint5 imm5) {
        rd = std::bit_cast<int32_t>(rs.as_u32() >> imm5);
    }

    void Interpreter::sraiw(GPIntReg &rd, const GPIntReg &rs, uint5 imm5) {
        rd = rs.as_i32() >> imm5;
    }

    void Interpreter::sllw(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) {
        rd = rs1.as_i32() << (rs2.val() & 0x1F);
    }

    void Interpreter::srlw(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) {
        rd = std::bit_cast<int32_t>(rs1.as_u32() >> (rs2.val() & 0x1F));
    }

    void Interpreter::sraw(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) {
        rd = rs1.as_i32() >> (rs2.val() & 0x1F);
    }

    void Interpreter::addw(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) {
        rd = rs1.as_i32() + rs2.as_i32();
    }

    void Interpreter::subw(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) {
        rd = rs1.as_i32() - rs2.as_i32();
    }

    void Interpreter::ld(GPIntReg &rd, const GPIntReg &rs, int12 imm12) {
        load_instruction_tmpl<int64_t>(rd, rs, imm12);
    }

    void Interpreter::lwu(GPIntReg &rd, const GPIntReg &rs, int12 imm12) {
        load_instruction_tmpl<uint32_t>(rd, rs, imm12);
    }

    void Interpreter::sd(const GPIntReg &rs2, const GPIntReg &rs1, int12 imm12) {
        store_instruction_tmpl<int64_t>(rs1, rs2, imm12);
    }

    void Interpreter::mul(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) {
        rd = rs1.sval() * rs2.sval();
    }

    void Interpreter::mulh(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) {
        // TODO: TEST ALL VARIANTS
#   ifdef __SIZEOF_INT128__
        rd = int64_t(__int128_t(rs1.sval()) * __int128_t(rs2.sval()) >> 64);
#   elif defined(_MSC_VER) && defined(_M_X64)
        rd = int64_t(__mulh(rs1.sval(), rs2.sval()));
#   else
        bool neg = (rs1.sval() < 0) ^ (rs2.sval() < 0);
        auto hi = mul64x64_128high(rs1.val(), rs2.val());

        if (neg) {
            auto low = rs1.val() * rs2.val();
            hi = ~hi + ((~low + 1) == 0 ? 1 : 0);
        }
        rd = hi;
#   endif
    }

    void Interpreter::mulhu(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) {
#   ifdef __SIZEOF_INT128__
        rd.val() = __uint128_t(rs1.val()) * __uint128_t(rs2.val()) >> 64;
#   elif defined(_MSC_VER) && defined(_M_X64)
        rd = __umulh(rs1.val(), rs2.val());
#   else
        rd = mul64x64_128high(rs1.val(), rs2.val());
#   endif
    }

    void Interpreter::mulhsu(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) {
#   ifdef __SIZEOF_INT128__
        rd = int64_t(__int128_t(rs1.sval()) * __uint128_t(rs2.val()) >> 64);
#   else
        bool neg = (rs1.sval() < 0);

#       if defined(_MSC_VER) && defined(_M_X64)
        uint64_t hi = __umulh(uint64_t(rs1.sval()), rs2.val());
#       else
        uint64_t hi = mul64x64_128high(rs1.val(), rs2.val());
#       endif

        if (neg) {
            uint64_t low = uint64_t(rs1.sval()) * rs2.val();
            hi = ~hi + ((~low + 1) == 0 ? 1 : 0);
        }

        rd = hi;
#   endif
    }

    void Interpreter::mulw(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) {
        rd = rs1.as_i32() * rs2.as_i32();
    }

    void Interpreter::div(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) {
        rd = div_rem_tmpl<false>(rs1.sval(), rs2.sval());
    }

    void Interpreter::divu(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) {
        if (rs2 == 0) {
            rd = -1;
        } else {
            rd = rs1.val() / rs2.val();
        }
    }

    void Interpreter::rem(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) {
        rd = div_rem_tmpl<true>(rs1.sval(), rs2.sval());
    }

    void Interpreter::remu(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) {
        if (rs2 == 0) {
            rd = rs1.val();
        } else {
            rd = rs1.val() % rs2.val();
        }
    }

    void Interpreter::divw(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) {
        rd = div_rem_tmpl(rs1.as_i32(), rs2.as_i32());
    }

    void Interpreter::divuw(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) {
        if (rs2.as_i32() == 0) {
            rd = -1;
            return;
        }
        rd = std::bit_cast<int32_t>(rs1.as_u32() / rs2.as_u32());
    }

    void Interpreter::remw(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) {
        rd = div_rem_tmpl<true>(rs1.as_i32(), rs2.as_i32());
    }

    void Interpreter::remuw(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) {
        if (rs2.as_i32() == 0) {
            rd = rs1.as_i32();
            return;
        }
        rd = std::bit_cast<int32_t>(rs1.as_u32() % rs2.as_u32());
    }

    void Interpreter::c_lwsp(GPIntReg &rd, int6 imm6) {
        if (rd.idx() == 0) {
            handle_error("destination register cannot be x0 in c.lwsp instruction");
            return;
        }

        auto offset = int64_t(imm6) << 2;
        load_instruction_tmpl<int32_t>(rd, x2_reg(), offset);
    }

    void Interpreter::c_ldsp(GPIntReg &rd, int6 imm6) {
        if (rd.idx() == 0) {
            handle_error("destination register cannot be x0 in c.lwsp instruction");
            return;
        }
        auto offset = int64_t(imm6) << 3;
        load_instruction_tmpl<int64_t>(rd, x2_reg(), offset);
    }

    void Interpreter::c_fldsp(GPIntReg &rd, int6 imm6) {
        handle_error("Floating point instructions are not handled: c.fldsp");
    }

    // C.SWSP stores a 32-bit value in register rs2 to memory.
    // It computes an effective address by adding the zero-extended offset,
    // scaled by 4, to the stack pointer, x2. It expands to sw rs2, offset(x2).
    void Interpreter::c_swsp(const GPIntReg &rs2, int6 imm6) {
        auto offset = uint64_t(imm6) << 2;
        store_instruction_tmpl<int32_t>(x2_reg(), rs2, offset);
    }

    void Interpreter::c_sdsp(const GPIntReg &rs2, int6 imm6) {
        auto offset = int64_t(imm6) << 3;
        store_instruction_tmpl<uint64_t>(x2_reg(), rs2, offset);
    }

    void Interpreter::c_fsdsp(GPIntReg &rd, int6 imm6) {
        handle_error("Floating point instructions are not handled: c.fsdsp");
    }

    void Interpreter::c_lw(GPIntReg &rdp, const GPIntReg &rs1p, int5 imm5) {
        assert(rdp.in_compressed_range() && rs1p.in_compressed_range());

        auto offset = int64_t(imm5) << 2;
        load_instruction_tmpl<int32_t>(rdp, rs1p, offset);
    }

    void Interpreter::c_ld(GPIntReg &rdp, const GPIntReg &rs1p, int5 imm5) {
        assert(rdp.in_compressed_range() && rs1p.in_compressed_range());

        auto offset = int64_t(imm5) << 3;
        load_instruction_tmpl<uint64_t>(rdp, rs1p, offset);
    }

    void Interpreter::c_fld(GPIntReg &rdp, const GPIntReg &rs1p, int5 imm5) {
        handle_error("Floating point instructions are not handled: c.fld");
    }

    void Interpreter::c_sw(const GPIntReg &rs2p, const GPIntReg &rs1p, int5 imm5) {
        auto offset = int64_t(imm5) << 2;
        store_instruction_tmpl<uint32_t>(rs1p, rs2p, offset);
    }

    void Interpreter::c_sd(const GPIntReg &rs2p, const GPIntReg &rs1p, int5 imm5) {
        auto offset = int64_t(imm5) << 3;
        store_instruction_tmpl<uint64_t>(rs1p, rs2p, offset);
    }

    void Interpreter::c_fsd(const GPIntReg &rs2p, const GPIntReg &rs1p, int5 imm5) {
        handle_error("Floating point instructions are not handled: c.fsd");
    }

    void Interpreter::c_j(int11 imm11) {
        // Offset is relative to current_pc, but PC already advanced by 2
        auto offset = int64_t(imm11) << 1;
        m_vm.m_cpu.move_pc(offset - 2);
    }

    void Interpreter::c_jr(const GPIntReg &rs1) {
        if (rs1.idx() == 0) {
            handle_error("destination register cannot be x0 in c.jr");
            return;
        }
        m_vm.m_cpu.set_pc(rs1.val());
    }

    void Interpreter::c_jalr(const GPIntReg &rs1) {
        if (rs1.idx() == 0) {
            ebreak();
            return;
        }
        m_vm.m_cpu.reg(1) = m_vm.m_cpu.get_pc();
        m_vm.m_cpu.set_pc(rs1.val());
    }

    void Interpreter::c_beqz(const GPIntReg &rs1p, int8 imm8) {
        assert(rs1p.in_compressed_range());
        if (rs1p == 0) {
            // Offset is relative to current_pc, but PC already advanced by 2
            auto offset = int64_t(imm8) << 1;
            m_vm.m_cpu.move_pc(offset - 2);
        }
    }

    void Interpreter::c_bnez(const GPIntReg &rs1p, int8 imm8) {
        assert(rs1p.in_compressed_range());
        if (rs1p != 0) {
            // Offset is relative to current_pc, but PC already advanced by 2
            auto offset = int64_t(imm8) << 1;
            m_vm.m_cpu.move_pc(offset - 2);
        }
    }

    void Interpreter::c_li(GPIntReg &rd, int6 imm6) {
        if (rd.idx() == 0)
            ui::print_hint("Loading immediate into x0 has no effect.");

        rd = int64_t(imm6);
    }

    void Interpreter::c_lui(GPIntReg &rd, int6 nzimm6) {
        if (rd.idx() == 0) {
            ui::print_hint("Loading immediate into x0 has no effect.");
        } else if (rd.idx() == 2) {
            c_addi16sp(rd, nzimm6);
            return;
        }
        if (nzimm6 == 0) {
            handle_error("nzimm6 cannot be zero in c.lui instruction");
            return;
        }

        rd = int64_t(nzimm6) << 12;
    }

    void Interpreter::c_addi(GPIntReg &rd, int6 nzimm6) {
        if (rd.idx() == 0) return; // C.NOP
        if (nzimm6 == 0) {
            ui::print_hint("Adding zero immediate has no effect.");
        }
        rd.sval() += nzimm6;
    }

    void Interpreter::c_addiw(GPIntReg &rd, int6 nzimm6) {
        if (rd.idx() == 0) {
            handle_error("Register cannot be x0 in c.addiw instruction");
            return;
        }
        rd = rd.as_i32() + nzimm6.as_i32();
    }

    void Interpreter::c_addi16sp(GPIntReg &x2, int6 nzimm6) {
        if (nzimm6 == 0) {
            handle_error("nzimm6 cannot be zero in c.addi16sp instruction");
            return;
        }
        if (x2.idx() != 2) {
            c_lui(x2, nzimm6);
            return;
        }
        x2_reg() = x2_reg().sval() + (int64_t(nzimm6) << 4);
    }

    void Interpreter::c_addi4spn(GPIntReg &rdp, uint8 nzuimm8) {
        assert(rdp.in_compressed_range());
        if (nzuimm8 == 0) {
            handle_error("nzuimm8 cannot be zero in c.addi4spn instruction");
            return;
        }
        rdp = x2_reg().sval() + (int64_t(nzuimm8) << 2);
    }

    void Interpreter::c_slli(GPIntReg &rd, uint6 nzuimm6) {
        if (nzuimm6 == 0) {
            ui::print_hint("Shifting by zero has no effect.");
            return;
        }
        if (rd.idx() == 0) {
            ui::print_hint("Shifting zero register has no effect.");
            return;
        }
        rd = rd.val() << nzuimm6;
    }

    void Interpreter::c_srli(GPIntReg &rdp, uint6 nzuimm6) {
        assert(rdp.in_compressed_range());
        if (nzuimm6 == 0) {
            ui::print_hint("Shifting by zero has no effect.");
            return;
        }
        if (rdp.idx() == 0) {
            ui::print_hint("Shifting zero register has no effect.");
            return;
        }
        rdp = rdp.val() >> nzuimm6;
    }

    void Interpreter::c_srai(GPIntReg &rdp, uint6 nzuimm6) {
        assert(rdp.in_compressed_range());
        if (nzuimm6 == 0) {
            ui::print_hint("Shifting by zero has no effect.");
            return;
        }
        if (rdp.idx() == 0) {
            ui::print_hint("Shifting zero register has no effect.");
            return;
        }
        rdp = rdp.sval() >> nzuimm6; // signed int shifting is arithmetic in C++ 20
    }

    void Interpreter::c_andi(GPIntReg &rdp, int6 imm6) {
        rdp = rdp.sval() & int64_t(imm6);
    }

    void Interpreter::c_mv(GPIntReg &rd, const GPIntReg &rs2) {
        if (rs2.idx() == 0) {
            c_jr(rs2);
            return;
        }
        if (rd.idx() == 0) {
            ui::print_hint("Moving to zero register has no effect.");
            return;
        }
        rd = rs2;
    }

    void Interpreter::c_add(GPIntReg &rd, const GPIntReg &rs2) {
        if (rs2.idx() == 0) {
            c_jalr(rs2);
            return;
        }
        if (rd.idx() == 0) {
            ui::print_hint("Adding to zero register has no effect.");
            return;
        }
        rd = rd.sval() + rs2.sval();
    }

    void Interpreter::c_and(GPIntReg &rdp, const GPIntReg &rs2p) {
        assert(rdp.in_compressed_range() && rs2p.in_compressed_range());
        rdp = rdp.val() & rs2p.val();
    }

    void Interpreter::c_or(GPIntReg &rdp, const GPIntReg &rs2p) {
        assert(rdp.in_compressed_range() && rs2p.in_compressed_range());
        rdp = rdp.val() | rs2p.val();
    }

    void Interpreter::c_xor(GPIntReg &rdp, const GPIntReg &rs2p) {
        assert(rdp.in_compressed_range() && rs2p.in_compressed_range());
        rdp = rdp.val() ^ rs2p.val();
    }

    void Interpreter::c_sub(GPIntReg &rdp, const GPIntReg &rs2p) {
        assert(rdp.in_compressed_range() && rs2p.in_compressed_range());
        rdp = rdp.sval() - rs2p.sval();
    }

    void Interpreter::c_addw(GPIntReg &rdp, const GPIntReg &rs2p) {
        assert(rdp.in_compressed_range() && rs2p.in_compressed_range());
        rdp = rdp.as_i32() + rs2p.as_i32();
    }

    void Interpreter::c_subw(GPIntReg &rdp, const GPIntReg &rs2p) {
        assert(rdp.in_compressed_range() && rs2p.in_compressed_range());
        rdp = rdp.as_i32() - rs2p.as_i32();
    }

    void Interpreter::handle_error(MemErr err) const {
        assert(err != MemErr::None);
        ui::print_error("Memory access error: " + Memory::err_to_string(err));
        m_vm.error_stop();
    }

    void Interpreter::handle_error(std::string_view msg) const {
        ui::print_error(msg);
        m_vm.error_stop();
    }

    uint64_t Interpreter::mul64x64_128high(uint64_t a, uint64_t b) {
        auto a_lo = a & UINT32_MAX, a_hi = a >> 32;
        auto b_lo = b & UINT32_MAX, b_hi = b >> 32;

        auto p0 = a_lo * b_lo;
        auto p1 = a_lo * b_hi;
        auto p2 = a_hi * b_lo;
        auto p3 = a_hi * b_hi;

        uint64_t carry = ((p0 >> 32) + (p1 & UINT32_MAX) + (p2 & UINT32_MAX)) >> 32;
        uint64_t mid = (p1 >> 32) + (p2 >> 32) + carry;
        return p3 + mid;
    }

    GPIntReg &Interpreter::x2_reg() const {
        return m_vm.m_cpu.reg(2);
    }

    template<typename T, typename TOff>
    void Interpreter::load_instruction_tmpl(GPIntReg &rd, const GPIntReg &rs, TOff offset) const {
        MemErr err;
        if constexpr (std::is_same_v<T, uint64_t>) {
            rd = m_vm.m_memory.load<T>(rs.sval() + offset, err);
        } else {
            rd = int64_t(m_vm.m_memory.load<T>(rs.sval() + offset, err));
        }
        if (err != MemErr::None)
            handle_error(err);
    }


    template<typename T, typename TOff>
    void Interpreter::store_instruction_tmpl(const GPIntReg &rs, const GPIntReg &rs2, TOff offset) {
        auto err = m_vm.m_memory.store<T>(rs.sval() + offset, static_cast<T>(rs2.val()));
        if (err != MemErr::None)
            handle_error(err);
    }

    template<bool Rem, typename T>
    int64_t Interpreter::div_rem_tmpl(T lhs, T rhs) {
        static_assert(std::is_signed_v<T>);
        if (rhs == 0)
            return Rem ? lhs : -1;
        if (lhs == std::numeric_limits<T>::min() && rhs == -1)
            return Rem ? 0 : lhs;
        return Rem ? (lhs % rhs) : (lhs / rhs);
    }

    void Interpreter::exec_instruction(const Instruction &in) {
        if (!in.is_valid()) {
            auto pc = m_vm.m_cpu.get_pc();
            ui::print_error(std::format(
                    "invalid instruction\n"
                    " - pc = 0x{:x} ({})\n"
                    " - line = {}\n",
                    pc, pc,
                    get_current_line()
                )
            );
            m_vm.error_stop();
            return;
        }

        auto args = in.get_args();

        auto get_reg = [this](InstArg arg) -> GPIntReg & {
            return m_vm.m_cpu.reg(std::get<Reg>(arg));
        };

        switch (in.get_prototype().id) {
            case (int) IBaseI::InstId::addi:
                addi(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;
            case (int) IBaseI::InstId::slti:
                slti(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;
            case (int) IBaseI::InstId::sltiu:
                sltiu(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;
            case (int) IBaseI::InstId::andi:
                andi(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;
            case (int) IBaseI::InstId::ori:
                ori(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;
            case (int) IBaseI::InstId::xori:
                xori(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;

            // --- Shift immediate ---
            case (int) IBaseI::InstId::slli:
                slli(get_reg(args[0]), get_reg(args[1]), std::get<uint6>(args[2]));
                break;
            case (int) IBaseI::InstId::srli:
                srli(get_reg(args[0]), get_reg(args[1]), std::get<uint6>(args[2]));
                break;
            case (int) IBaseI::InstId::srai:
                srai(get_reg(args[0]), get_reg(args[1]), std::get<uint6>(args[2]));
                break;

            // --- U-type ---
            case (int) IBaseI::InstId::lui:
                lui(get_reg(args[0]), std::get<int20>(args[1]));
                break;
            case (int) IBaseI::InstId::auipc:
                auipc(get_reg(args[0]), std::get<int20>(args[1]));
                break;

            // --- R-type arithmetic ---
            case (int) IBaseI::InstId::add:
                add(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;
            case (int) IBaseI::InstId::sub:
                sub(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;
            case (int) IBaseI::InstId::sll:
                sll(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;
            case (int) IBaseI::InstId::slt:
                slt(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;
            case (int) IBaseI::InstId::sltu:
                sltu(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;
            case (int) IBaseI::InstId::sra:
                sra(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;
            case (int) IBaseI::InstId::srl:
                srl(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;
            case (int) IBaseI::InstId::and_:
                and_(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;
            case (int) IBaseI::InstId::or_:
                or_(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;
            case (int) IBaseI::InstId::xor_:
                xor_(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;

            // --- Branches ---
            case (int) IBaseI::InstId::beq:
                beq(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;
            case (int) IBaseI::InstId::bne:
                bne(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;
            case (int) IBaseI::InstId::blt:
                blt(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;
            case (int) IBaseI::InstId::bge:
                bge(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;
            case (int) IBaseI::InstId::bltu:
                bltu(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;
            case (int) IBaseI::InstId::bgeu:
                bgeu(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;

            // --- Jumps ---
            case (int) IBaseI::InstId::jal:
                jal(get_reg(args[0]), std::get<int20>(args[1]));
                break;
            case (int) IBaseI::InstId::jalr:
                jalr(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;

            // --- Memory ---
            case (int) IBaseI::InstId::lw:
                lw(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;
            case (int) IBaseI::InstId::lh:
                lh(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;
            case (int) IBaseI::InstId::lhu:
                lhu(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;
            case (int) IBaseI::InstId::lb:
                lb(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;
            case (int) IBaseI::InstId::lbu:
                lbu(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;
            case (int) IBaseI::InstId::sw:
                sw(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;
            case (int) IBaseI::InstId::sh:
                sh(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;
            case (int) IBaseI::InstId::sb:
                sb(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;
            case (int) IBaseI::InstId::ld:
                ld(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;
            case (int) IBaseI::InstId::lwu:
                lwu(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;
            case (int) IBaseI::InstId::sd:
                sd(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;

            // --- RV64I word operations ---
            case (int) IBaseI::InstId::addiw:
                addiw(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;
            case (int) IBaseI::InstId::slliw:
                slliw(get_reg(args[0]), get_reg(args[1]), std::get<uint5>(args[2]));
                break;
            case (int) IBaseI::InstId::srliw:
                srliw(get_reg(args[0]), get_reg(args[1]), std::get<uint5>(args[2]));
                break;
            case (int) IBaseI::InstId::sraiw:
                sraiw(get_reg(args[0]), get_reg(args[1]), std::get<uint5>(args[2]));
                break;
            case (int) IBaseI::InstId::addw:
                addw(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;
            case (int) IBaseI::InstId::subw:
                subw(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;
            case (int) IBaseI::InstId::sllw:
                sllw(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;
            case (int) IBaseI::InstId::srlw:
                srlw(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;
            case (int) IBaseI::InstId::sraw:
                sraw(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;

            // --- System ---
            case (int) IBaseI::InstId::fence:
                fence();
                break;
            case (int) IBaseI::InstId::ecall:
                ecall();
                break;
            case (int) IBaseI::InstId::ebreak:
                ebreak();
                break;

            // --- M-extension ---
            case (int) IExtensionM::InstId::mul:
                mul(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;
            case (int) IExtensionM::InstId::mulh:
                mulh(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;
            case (int) IExtensionM::InstId::mulhu:
                mulhu(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;
            case (int) IExtensionM::InstId::mulhsu:
                mulhsu(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;
            case (int) IExtensionM::InstId::mulw:
                mulw(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;
            case (int) IExtensionM::InstId::div:
                div(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;
            case (int) IExtensionM::InstId::divu:
                divu(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;
            case (int) IExtensionM::InstId::rem:
                rem(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;
            case (int) IExtensionM::InstId::remu:
                remu(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;
            case (int) IExtensionM::InstId::divw:
                divw(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;
            case (int) IExtensionM::InstId::divuw:
                divuw(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;
            case (int) IExtensionM::InstId::remw:
                remw(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;
            case (int) IExtensionM::InstId::remuw:
                remuw(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;

            // --- C-extension ---
            case (int) IExtensionC::InstId::c_lwsp:
                c_lwsp(get_reg(args[0]), std::get<int6>(args[1]));
                break;
            case (int) IExtensionC::InstId::c_ldsp:
                c_ldsp(get_reg(args[0]), std::get<int6>(args[1]));
                break;
            case (int) IExtensionC::InstId::c_fldsp:
                c_fldsp(get_reg(args[0]), std::get<int6>(args[1]));
                break;
            case (int) IExtensionC::InstId::c_swsp:
                c_swsp(get_reg(args[0]), std::get<int6>(args[1]));
                break;
            case (int) IExtensionC::InstId::c_sdsp:
                c_sdsp(get_reg(args[0]), std::get<int6>(args[1]));
                break;
            case (int) IExtensionC::InstId::c_fsdsp:
                c_fsdsp(get_reg(args[0]), std::get<int6>(args[1]));
                break;
            case (int) IExtensionC::InstId::c_lw:
                c_lw(get_reg(args[0]), get_reg(args[1]), std::get<int5>(args[2]));
                break;
            case (int) IExtensionC::InstId::c_ld:
                c_ld(get_reg(args[0]), get_reg(args[1]), std::get<int5>(args[2]));
                break;
            case (int) IExtensionC::InstId::c_fld:
                c_fld(get_reg(args[0]), get_reg(args[1]), std::get<int5>(args[2]));
                break;
            case (int) IExtensionC::InstId::c_sw:
                c_sw(get_reg(args[0]), get_reg(args[1]), std::get<int5>(args[2]));
                break;
            case (int) IExtensionC::InstId::c_sd:
                c_sd(get_reg(args[0]), get_reg(args[1]), std::get<int5>(args[2]));
                break;
            case (int) IExtensionC::InstId::c_fsd:
                c_fsd(get_reg(args[0]), get_reg(args[1]), std::get<int5>(args[2]));
                break;
            case (int) IExtensionC::InstId::c_j:
                c_j(std::get<int11>(args[0]));
                break;
            case (int) IExtensionC::InstId::c_jr:
                c_jr(get_reg(args[0]));
                break;
            case (int) IExtensionC::InstId::c_jalr:
                c_jalr(get_reg(args[0]));
                break;
            case (int) IExtensionC::InstId::c_beqz:
                c_beqz(get_reg(args[0]), std::get<int8>(args[1]));
                break;
            case (int) IExtensionC::InstId::c_bnez:
                c_bnez(get_reg(args[0]), std::get<int8>(args[1]));
                break;
            case (int) IExtensionC::InstId::c_li:
                c_li(get_reg(args[0]), std::get<int6>(args[1]));
                break;
            case (int) IExtensionC::InstId::c_lui:
                c_lui(get_reg(args[0]), std::get<int6>(args[1]));
                break;
            case (int) IExtensionC::InstId::c_addi:
                c_addi(get_reg(args[0]), std::get<int6>(args[1]));
                break;
            case (int) IExtensionC::InstId::c_addiw:
                c_addiw(get_reg(args[0]), std::get<int6>(args[1]));
                break;
            case (int) IExtensionC::InstId::c_addi16sp:
                c_addi16sp(get_reg(args[0]), std::get<int6>(args[1]));
                break;
            case (int) IExtensionC::InstId::c_addi4spn:
                c_addi4spn(get_reg(args[0]), std::get<uint8>(args[1]));
                break;
            case (int) IExtensionC::InstId::c_slli:
                c_slli(get_reg(args[0]), std::get<uint6>(args[1]));
                break;
            case (int) IExtensionC::InstId::c_srli:
                c_srli(get_reg(args[0]), std::get<uint6>(args[1]));
                break;
            case (int) IExtensionC::InstId::c_srai:
                c_srai(get_reg(args[0]), std::get<uint6>(args[1]));
                break;
            case (int) IExtensionC::InstId::c_andi:
                c_andi(get_reg(args[0]), std::get<int6>(args[1]));
                break;
            case (int) IExtensionC::InstId::c_mv:
                c_mv(get_reg(args[0]), get_reg(args[1]));
                break;
            case (int) IExtensionC::InstId::c_add:
                c_add(get_reg(args[0]), get_reg(args[1]));
                break;
            case (int) IExtensionC::InstId::c_and:
                c_and(get_reg(args[0]), get_reg(args[1]));
                break;
            case (int) IExtensionC::InstId::c_or:
                c_or(get_reg(args[0]), get_reg(args[1]));
                break;
            case (int) IExtensionC::InstId::c_xor:
                c_xor(get_reg(args[0]), get_reg(args[1]));
                break;
            case (int) IExtensionC::InstId::c_sub:
                c_sub(get_reg(args[0]), get_reg(args[1]));
                break;
            case (int) IExtensionC::InstId::c_addw:
                c_addw(get_reg(args[0]), get_reg(args[1]));
                break;
            case (int) IExtensionC::InstId::c_subw:
                c_subw(get_reg(args[0]), get_reg(args[1]));
                break;

            // nops
            case (int) IBaseI::InstId::nop:
                nop();
                break;
            case (int) IExtensionC::InstId::c_nop:
                c_nop();
                break;

            default:
                throw std::runtime_error(std::format("Unknown instruction ID: {}", in.get_prototype().id));
        }
        assert(m_vm.m_cpu.reg(0) == 0); // x0 is always zero
    }
}
