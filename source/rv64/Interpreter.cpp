#include "Interpreter.hpp"

#include <any>
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
    template void Interpreter::load_instruction_tmpl<int64_t>(GPIntReg &rd, const GPIntReg &rs, int12 imm12);
    template void Interpreter::load_instruction_tmpl<uint32_t>(GPIntReg &rd, const GPIntReg &rs, int12 imm12);
    template void Interpreter::load_instruction_tmpl<uint16_t>(GPIntReg &rd, const GPIntReg &rs, int12 imm12);
    template void Interpreter::load_instruction_tmpl<int16_t>(GPIntReg &rd, const GPIntReg &rs, int12 imm12);
    template void Interpreter::load_instruction_tmpl<uint8_t>(GPIntReg &rd, const GPIntReg &rs, int12 imm12);
    template void Interpreter::load_instruction_tmpl<int8_t>(GPIntReg &rd, const GPIntReg &rs, int12 imm12);


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
        rd = imm20 << 12;
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
        rd = m_vm.m_cpu.get_pc() + 4;
        m_vm.m_cpu.set_pc(rd.val() + imm20 * 2 - 4);
    }

    void Interpreter::jalr(GPIntReg &rd, const GPIntReg &rs, int12 imm12) {
        rd = m_vm.m_cpu.get_pc() + 4;
        m_vm.m_cpu.set_pc(rs.sval() + imm12 * 2 - 4);
    }

    void Interpreter::beq(const GPIntReg &rs1, const GPIntReg &rs2, int12 imm12) {
        if (rs1.sval() == rs2.sval()) {
            m_vm.m_cpu.set_pc(m_vm.m_cpu.get_pc() + (imm12 * 2) - 4);
        }
    }

    void Interpreter::bne(const GPIntReg &rs1, const GPIntReg &rs2, int12 imm12) {
        if (rs1.sval() != rs2.sval()) {
            m_vm.m_cpu.set_pc(m_vm.m_cpu.get_pc() + (imm12 * 2) - 4);
        }
    }

    void Interpreter::blt(const GPIntReg &rs1, const GPIntReg &rs2, int12 imm12) {
        if (rs1.sval() < rs2.sval()) {
            m_vm.m_cpu.set_pc(m_vm.m_cpu.get_pc() + (imm12 * 2) - 4);
        }
    }

    void Interpreter::bltu(const GPIntReg &rs1, const GPIntReg &rs2, int12 imm12) {
        if (rs1.val() < rs2.val()) {
            m_vm.m_cpu.set_pc(m_vm.m_cpu.get_pc() + imm12 * 2 - 4);
        }
    }

    void Interpreter::bge(const GPIntReg &rs1, const GPIntReg &rs2, int12 imm12) {
        if (rs1.sval() >= rs2.sval()) {
            m_vm.m_cpu.set_pc(m_vm.m_cpu.get_pc() + imm12 * 2 - 4);
        }
    }

    void Interpreter::bgeu(const GPIntReg &rs1, const GPIntReg &rs2, int12 imm12) {
        if (rs1.val() >= rs2.val()) {
            m_vm.m_cpu.set_pc(m_vm.m_cpu.get_pc() + imm12 * 2 - 4);
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

    void Interpreter::sw(const GPIntReg &rs, const GPIntReg &rs2, int12 imm12) {
        store_instruction_tmpl<uint32_t>(rs, rs2, imm12);
    }

    void Interpreter::sh(const GPIntReg &rs, const GPIntReg &rs2, int12 imm12) {
        store_instruction_tmpl<uint16_t>(rs, rs2, imm12);
    }

    void Interpreter::sb(const GPIntReg &rs, const GPIntReg &rs2, int12 imm12) {
        store_instruction_tmpl<uint8_t>(rs, rs2, imm12);
    }

    void Interpreter::fence() {
        // Implementation is not needed for single threaded systems.
    }

    void Interpreter::ecall() {
        auto &a0 = m_vm.m_cpu.get_reg(10);
        const auto &a1 = m_vm.m_cpu.get_reg(11);
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

    void Interpreter::addiw(GPIntReg &rd, const GPIntReg &rs, int12 imm12) {
        rd = rs.as_i32() + imm12;
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

    void Interpreter::sd(const GPIntReg &rs1, const GPIntReg &rs2, int12 imm12) {
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

    void Interpreter::mulw(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) {
        rd = rs1.as_i32() * rs2.as_i32();
    }

    void Interpreter::div(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) {
        rd = rs1.sval() / rs2.sval();
    }

    void Interpreter::divu(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) {
        rd = rs1.val() / rs2.val();
    }

    void Interpreter::rem(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) {
        rd = div_rem_tmpl<true>(rs1.sval(), rs2.sval());
    }

    void Interpreter::remu(GPIntReg &rd, const GPIntReg &rs1, const GPIntReg &rs2) {
        if (rs2.sval() == 0) {
            rd = rs1.val();
            return;
        }
        rd = rs1.val() % rs2.val();
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
    void Interpreter::load_instruction_tmpl(GPIntReg &rd, const GPIntReg &rs, int12 imm12) {
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
    void Interpreter::store_instruction_tmpl(const GPIntReg &rs, const GPIntReg &rs2, int12 imm12) {
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

    void Interpreter::exec_instruction(const Instruction &in) {
        assert(in.is_valid()); // assure that arguments are valid
        auto args = in.get_args();

        auto get_reg = [this](InstArg arg) -> GPIntReg& {
            return m_vm.m_cpu.get_int_reg(std::get<Reg>(arg));
        };
        auto proto = in.get_prototype();

        switch (in.get_prototype().id) {
            // --- Immediate arithmetic ---
            case (int) IBase::InstId::addi:
                addi(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;
            case (int) IBase::InstId::slti:
                slti(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;
            case (int) IBase::InstId::sltiu:
                sltiu(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;
            case (int) IBase::InstId::andi:
                andi(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;
            case (int) IBase::InstId::ori:
                ori(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;
            case (int) IBase::InstId::xori:
                xori(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;

            // --- Shift immediate ---
            case (int) IBase::InstId::slli:
                slli(get_reg(args[0]), get_reg(args[1]), std::get<uint6>(args[2]));
                break;
            case (int) IBase::InstId::srli:
                srli(get_reg(args[0]), get_reg(args[1]), std::get<uint6>(args[2]));
                break;
            case (int) IBase::InstId::srai:
                srai(get_reg(args[0]), get_reg(args[1]), std::get<uint6>(args[2]));
                break;

            // --- U-type ---
            case (int) IBase::InstId::lui:
                lui(get_reg(args[0]), std::get<int20>(args[1]));
                break;
            case (int) IBase::InstId::auipc:
                auipc(get_reg(args[0]), std::get<int20>(args[1]));
                break;

            // --- R-type arithmetic ---
            case (int) IBase::InstId::add:
                add(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;
            case (int) IBase::InstId::sub:
                sub(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;
            case (int) IBase::InstId::sll:
                sll(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;
            case (int) IBase::InstId::slt:
                slt(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;
            case (int) IBase::InstId::sra:
                sra(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;
            case (int) IBase::InstId::srl:
                srl(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;
            case (int) IBase::InstId::and_:
                and_(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;
            case (int) IBase::InstId::or_:
                or_(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;
            case (int) IBase::InstId::xor_:
                xor_(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;

            // --- Branches ---
            case (int) IBase::InstId::beq:
                beq(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;
            case (int) IBase::InstId::bne:
                bne(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;
            case (int) IBase::InstId::blt:
                blt(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;
            case (int) IBase::InstId::bge:
                bge(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;
            case (int) IBase::InstId::bltu:
                bltu(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;
            case (int) IBase::InstId::bgeu:
                bgeu(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;

            // --- Jumps ---
            case (int) IBase::InstId::jal:
                jal(get_reg(args[0]), std::get<int20>(args[1]));
                break;
            case (int) IBase::InstId::jalr:
                jalr(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;

            // --- Memory ---
            case (int) IBase::InstId::lw:
                lw(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;
            case (int) IBase::InstId::lh:
                lh(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;
            case (int) IBase::InstId::lhu:
                lhu(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;
            case (int) IBase::InstId::lb:
                lb(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;
            case (int) IBase::InstId::lbu:
                lbu(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;
            case (int) IBase::InstId::sw:
                sw(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;
            case (int) IBase::InstId::sh:
                sh(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;
            case (int) IBase::InstId::sb:
                sb(get_reg(args[0]), get_reg(args[1]), std::get<int12>(args[2]));
                break;

            // --- System ---
            case (int) IBase::InstId::fence:
                fence();
                break;
            case (int) IBase::InstId::ecall:
                ecall();
                break;
            case (int) IBase::InstId::ebreak:
                ebreak();
                break;

            // --- M-extension ---
            case (int) IExtM::InstId::mul:
                mul(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;
            case (int) IExtM::InstId::mulh:
                mulh(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;
            case (int) IExtM::InstId::mulhu:
                mulhu(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;
            case (int) IExtM::InstId::mulhsu:
                mulhsu(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;
            case (int) IExtM::InstId::mulw:
                mulw(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;
            case (int) IExtM::InstId::div:
                div(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;
            case (int) IExtM::InstId::divu:
                divu(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;
            case (int) IExtM::InstId::rem:
                rem(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;
            case (int) IExtM::InstId::remu:
                remu(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;
            case (int) IExtM::InstId::divw:
                divw(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;
            case (int) IExtM::InstId::divuw:
                divuw(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;
            case (int) IExtM::InstId::remw:
                remw(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;
            case (int) IExtM::InstId::remuw:
                remuw(get_reg(args[0]), get_reg(args[1]), get_reg(args[2]));
                break;

            default:
                throw std::runtime_error(std::format("Unknown instruction ID: {}", in.get_prototype().id));
        }
        assert(m_vm.m_cpu.get_reg(0) == 0); // x0 is always zero
    }
}
