#include "Interpreter.hpp"

#include <cassert>
#include <format>
#include <rv64/Cpu.hpp>
#include <rv64/Memory.hpp>

#include "VM.hpp"

#if defined(_MSC_VER) && defined(_M_IX64)
#   include <intrin.h>
#   pragma intrinsic (__mulh)
#   pragma intrinsic (__umulh)
#endif

namespace rv64 {
    template void Interpreter::load_instruction_tmpl<int64_t>(IntReg &rd, const IntReg &rs, int12 imm12);
    template void Interpreter::load_instruction_tmpl<uint32_t>(IntReg &rd, const IntReg &rs, int12 imm12);
    template void Interpreter::load_instruction_tmpl<uint16_t>(IntReg &rd, const IntReg &rs, int12 imm12);
    template void Interpreter::load_instruction_tmpl<int16_t>(IntReg &rd, const IntReg &rs, int12 imm12);
    template void Interpreter::load_instruction_tmpl<uint8_t>(IntReg &rd, const IntReg &rs, int12 imm12);
    template void Interpreter::load_instruction_tmpl<int8_t>(IntReg &rd, const IntReg &rs, int12 imm12);


    void Interpreter::addi(IntReg &rd, const IntReg &rs, int12 imm12) {
        rd = rs.sval() + imm12;
    }

    void Interpreter::slti(IntReg &rd, const IntReg &rs, int12 imm12) {
        rd = rs.sval() < imm12 ? 1 : 0;
    }

    void Interpreter::sltiu(IntReg &rd, const IntReg &rs, int12 imm12) {
        rd = rs.val() < std::bit_cast<uint64_t>((int64_t)imm12) ? 1 : 0;
    }

    void Interpreter::andi(IntReg &rd, const IntReg &rs, int12 imm12) {
        rd = rs.sval() & imm12;
    }

    void Interpreter::ori(IntReg &rd, const IntReg &rs, int12 imm12) {
        rd = rs.sval() | imm12;
    }

    void Interpreter::xori(IntReg &rd, const IntReg &rs, int12 imm12) {
        rd = rs.sval() ^ imm12;
    }

    void Interpreter::slli(IntReg &rd, const IntReg &rs, uint6 uimm6) {
        rd = rs.val() << uimm6;
    }

    void Interpreter::srli(IntReg &rd, const IntReg &rs, uint6 uimm6) {
        rd = rs.val() >> uimm6;
    }

    void Interpreter::srai(IntReg &rd, const IntReg &rs, uint6 uimm6) {
        rd = rs.sval() >> uimm6; // C++ 20 defined behaviour
    }

    void Interpreter::lui(IntReg &rd, int20 imm20) {
        rd = imm20 << 12;
    }

    void Interpreter::auipc(IntReg &rd, int20 imm20) {
        rd = m_vm.m_cpu.get_pc() + (imm20 << 12);
    }

    void Interpreter::add(IntReg &rd, const IntReg &rs1, const IntReg &rs2) {
        rd = rs1.sval() + rs2.sval();
    }

    void Interpreter::sub(IntReg &rd, const IntReg &rs1, const IntReg &rs2) {
        rd = rs1.sval() - rs2.sval();
    }

    void Interpreter::slt(IntReg &rd, const IntReg &rs1, const IntReg &rs2) {
        rd = rs1.sval() < rs2.sval() ? 1 : 0;
    }

    void Interpreter::sltu(IntReg &rd, const IntReg &rs1, const IntReg &rs2) {
        rd = rs1.val() < rs2.val() ? 1 : 0;
    }

    void Interpreter::and_(IntReg &rd, const IntReg &rs1, const IntReg &rs2) {
        rd = rs1.val() & rs2.val();
    }

    void Interpreter::or_(IntReg &rd, const IntReg &rs1, const IntReg &rs2) {
        rd = rs1.val() | rs2.val();
    }

    void Interpreter::xor_(IntReg &rd, const IntReg &rs1, const IntReg &rs2) {
        rd = rs1.val() ^ rs2.val();
    }

    void Interpreter::sll(IntReg &rd, const IntReg &rs1, const IntReg &rs2) {
        rd = rs1.val() << (rs2.val() & 0x3F);
    }

    void Interpreter::srl(IntReg &rd, const IntReg &rs1, const IntReg &rs2) {
        rd = rs1.val() >> (rs2.val() & 0x3F);
    }

    void Interpreter::sra(IntReg &rd, const IntReg &rs1, const IntReg &rs2) {
        rd = rs1.sval() >> (rs2.val() & 0x3F); // C++ 20 defined behaviour
    }

    void Interpreter::jal(IntReg &rd, int20 imm20) {
        rd = m_vm.m_cpu.get_pc() + 4;
        m_vm.m_cpu.set_pc(rd.val() + imm20 * 2 - 4);
    }

    void Interpreter::jalr(IntReg &rd, const IntReg &rs, int12 imm12) {
        rd = m_vm.m_cpu.get_pc() + 4;
        m_vm.m_cpu.set_pc(rs.sval() + imm12 * 2 - 4);
    }

    void Interpreter::beq(const IntReg &rs1, const IntReg &rs2, int12 imm12) {
        if (rs1.sval() == rs2.sval()) {
            m_vm.m_cpu.set_pc(m_vm.m_cpu.get_pc() + (imm12 * 2) - 4);
        }
    }

    void Interpreter::bne(const IntReg &rs1, const IntReg &rs2, int12 imm12) {
        if (rs1.sval() != rs2.sval()) {
            m_vm.m_cpu.set_pc(m_vm.m_cpu.get_pc() + (imm12 * 2) - 4);
        }
    }

    void Interpreter::blt(const IntReg &rs1, const IntReg &rs2, int12 imm12) {
        if (rs1.sval() < rs2.sval()) {
            m_vm.m_cpu.set_pc(m_vm.m_cpu.get_pc() + (imm12 * 2) - 4);
        }
    }

    void Interpreter::bltu(const IntReg &rs1, const IntReg &rs2, int12 imm12) {
        if (rs1.val() < rs2.val()) {
            m_vm.m_cpu.set_pc(m_vm.m_cpu.get_pc() + imm12 * 2 - 4);
        }
    }

    void Interpreter::bge(const IntReg &rs1, const IntReg &rs2, int12 imm12) {
        if (rs1.sval() >= rs2.sval()) {
            m_vm.m_cpu.set_pc(m_vm.m_cpu.get_pc() + imm12 * 2 - 4);
        }
    }

    void Interpreter::bgeu(const IntReg &rs1, const IntReg &rs2, int12 imm12) {
        if (rs1.val() >= rs2.val()) {
            m_vm.m_cpu.set_pc(m_vm.m_cpu.get_pc() + imm12 * 2 - 4);
        }
    }

    void Interpreter::lw(IntReg &rd, const IntReg &rs, int12 imm12) {
        load_instruction_tmpl<int32_t>(rd, rs, imm12);
    }

    void Interpreter::lh(IntReg &rd, const IntReg &rs, int12 imm12) {
        load_instruction_tmpl<int16_t>(rd, rs, imm12);
    }

    void Interpreter::lhu(IntReg &rd, const IntReg &rs, int12 imm12) {
        load_instruction_tmpl<uint16_t>(rd, rs, imm12);
    }

    void Interpreter::lb(IntReg &rd, const IntReg &rs, int12 imm12) {
        load_instruction_tmpl<int8_t>(rd, rs, imm12);
    }

    void Interpreter::lbu(IntReg &rd, const IntReg &rs, int12 imm12) {
        load_instruction_tmpl<uint8_t>(rd, rs, imm12);
    }

    void Interpreter::sw(const IntReg &rs, const IntReg &rs2, int12 imm12) {
        store_instruction_tmpl<uint32_t>(rs, rs2, imm12);
    }

    void Interpreter::sh(const IntReg &rs, const IntReg &rs2, int12 imm12) {
        store_instruction_tmpl<uint16_t>(rs, rs2, imm12);
    }

    void Interpreter::sb(const IntReg &rs, const IntReg &rs2, int12 imm12) {
        store_instruction_tmpl<uint8_t>(rs, rs2, imm12);
    }

    void Interpreter::fence() {
        // Implementation is not needed for single threaded systems.
    }

    void Interpreter::ecall() {
        auto &a0 = m_vm.m_cpu.get_int_reg(10);
        const auto &a1 = m_vm.m_cpu.get_int_reg(11);
        MemErr err{};
        switch (a0.sval()) {
            case 1:
                gui::print_output(std::to_string(a1.sval()));
                return;
            case 4:
                gui::print_output(m_vm.m_memory.load_string(a1.val(), err));
                break;
            case 9:
                a0 = m_vm.m_memory.sbrk(a1.sval(), err);
                break;
            case 10:
                m_vm.terminate(0);
                return;
            case 11:
                gui::print_output(std::string(1, static_cast<char>(a1.val() & 0xFF)));
                return;
            case 17:
                m_vm.terminate(static_cast<int>(a1.sval()));
                return;
            default:
                gui::print_warning(std::format("Unsupported ecall code: {}", a0.sval()));
                return;
        }
        if (err != MemErr::None)
            handle_error(err);
    }

    void Interpreter::ebreak() {
        m_vm.breakpoint_hit();
    }

    void Interpreter::addiw(IntReg &rd, const IntReg &rs, int12 imm12) {
        rd = rs.as_i32() + imm12;
    }

    void Interpreter::slliw(IntReg &rd, const IntReg &rs, uint5 imm5) {
        rd = rs.as_i32() << imm5;
    }

    void Interpreter::srliw(IntReg &rd, const IntReg &rs, uint5 imm5) {
        rd = std::bit_cast<int32_t>(rs.as_u32() >> imm5);
    }

    void Interpreter::sraiw(IntReg &rd, const IntReg &rs, uint5 imm5) {
        rd = rs.as_i32() >> imm5;
    }

    void Interpreter::sllw(IntReg &rd, const IntReg &rs1, const IntReg &rs2) {
        rd = rs1.as_i32() << (rs2.val() & 0x1F);
    }

    void Interpreter::srlw(IntReg &rd, const IntReg &rs1, const IntReg &rs2) {
        rd = std::bit_cast<int32_t>(rs1.as_u32() >> (rs2.val() & 0x1F));
    }

    void Interpreter::sraw(IntReg &rd, const IntReg &rs1, const IntReg &rs2) {
        rd = rs1.as_i32() >> (rs2.val() & 0x1F);
    }

    void Interpreter::addw(IntReg &rd, const IntReg &rs1, const IntReg &rs2) {
        rd = rs1.as_i32() + rs2.as_i32();
    }

    void Interpreter::subw(IntReg &rd, const IntReg &rs1, const IntReg &rs2) {
        rd = rs1.as_i32() - rs2.as_i32();
    }

    void Interpreter::ld(IntReg &rd, const IntReg &rs, int12 imm12) {
        load_instruction_tmpl<int64_t>(rd, rs, imm12);
    }

    void Interpreter::lwu(IntReg &rd, const IntReg &rs, int12 imm12) {
        load_instruction_tmpl<uint32_t>(rd, rs, imm12);
    }

    void Interpreter::sd(const IntReg &rs1, const IntReg &rs2, int12 imm12) {
        store_instruction_tmpl<int64_t>(rs1, rs2, imm12);
    }

    void Interpreter::mul(IntReg &rd, const IntReg &rs1, const IntReg &rs2) {
        rd = rs1.sval() * rs2.sval();
    }

    void Interpreter::mulh(IntReg &rd, const IntReg &rs1, const IntReg &rs2) {
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

    void Interpreter::mulhu(IntReg &rd, const IntReg &rs1, const IntReg &rs2) {
#   ifdef __SIZEOF_INT128__
        rd.val() = __uint128_t(rs1.val()) * __uint128_t(rs2.val()) >> 64;
#   elif defined(_MSC_VER) && defined(_M_X64)
        rd = __umulh(rs1.val(), rs2.val());
#   else
        rd = mul64x64_128high(rs1.val(), rs2.val());
#   endif
    }

    void Interpreter::mulhsu(IntReg &rd, const IntReg &rs1, const IntReg &rs2) {
#   ifdef __SIZEOF_INT128__
        rd = int64_t(__int128_t(rs1.val()) * __uint128_t(rs2.val()) >> 64);
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

    void Interpreter::mulw(IntReg &rd, const IntReg &rs1, const IntReg &rs2) {
        rd = rs1.as_i32() * rs2.as_i32();
    }

    void Interpreter::div(IntReg &rd, const IntReg &rs1, const IntReg &rs2) {
        rd = rs1.sval() / rs2.sval();
    }

    void Interpreter::divu(IntReg &rd, const IntReg &rs1, const IntReg &rs2) {
        rd = rs1.val() / rs2.val();
    }

    void Interpreter::rem(IntReg &rd, const IntReg &rs1, const IntReg &rs2) {
        rd = div_rem_tmpl<true>(rs1.sval(), rs2.sval());
    }

    void Interpreter::remu(IntReg &rd, const IntReg &rs1, const IntReg &rs2) {
        if (rs2.sval() == 0) {
            rd = rs1.val();
            return;
        }
        rd = rs1.val() % rs2.val();
    }

    void Interpreter::divw(IntReg &rd, const IntReg &rs1, const IntReg &rs2) {
        rd = div_rem_tmpl(rs1.as_i32(), rs2.as_i32());
    }

    void Interpreter::divuw(IntReg &rd, const IntReg &rs1, const IntReg &rs2) {
        if (rs2.as_i32() == 0) {
            rd = -1;
            return;
        }
        rd = std::bit_cast<int32_t>(rs1.as_u32() / rs2.as_u32());
    }

    void Interpreter::remw(IntReg &rd, const IntReg &rs1, const IntReg &rs2) {
        rd = div_rem_tmpl<true>(rs1.as_i32(), rs2.as_i32());
    }

    void Interpreter::remuw(IntReg &rd, const IntReg &rs1, const IntReg &rs2) {
        if (rs2.as_i32() == 0) {
            rd = rs1.as_i32();
            return;
        }
        rd = std::bit_cast<int32_t>(rs1.as_u32() % rs2.as_u32());
    }

    void Interpreter::handle_error(MemErr err) const {
        assert(err != MemErr::None);
        gui::display_error("Memory access error: " + Memory::err_to_string(err));
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

    template<typename T>
    void Interpreter::load_instruction_tmpl(IntReg &rd, const IntReg &rs, int12 imm12) {
        MemErr err;
        if constexpr (std::is_same_v<T, uint64_t>) {
            rd = m_vm.m_memory.load<T>(rs.sval() + imm12, err);
        } else {
            rd = int64_t(m_vm.m_memory.load<T>(rs.sval() + imm12, err));
        }
        if (err != MemErr::None)
            handle_error(err);
    }

    template<typename T>
    void Interpreter::store_instruction_tmpl(const IntReg &rs, const IntReg &rs2, int12 imm12) {
        auto err = m_vm.m_memory.store<T>(rs.sval() + imm12, static_cast<T>(rs2.val()));
        if (err != MemErr::None)
            handle_error(err);
    }

    template<bool Rem, typename T>
    int64_t Interpreter::div_rem_tmpl(T lhs, T rhs) {
        static_assert(std::is_signed_v<T>);
        if (rhs == 0)
            return -1;
        if (lhs == std::numeric_limits<T>::min() && rhs == -1)
            return Rem ? 0 : lhs;
        return lhs / rhs;
    }
}
