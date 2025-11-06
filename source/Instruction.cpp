#include <cassert>
#include <Instruction.hpp>
#include <rv64/instruction_sets/Rv64IMC.hpp>

#include "rv64/Cpu.hpp"

template<typename T>
concept HasMIN = requires { T::MIN; };

template<typename T>
static bool in_range(const std::string &value) {
    if constexpr (!HasMIN<T>)
        return T::MAX >= std::stoull(value);

    return T::MAX >= std::stoll(value)
           && T::MIN <= std::stoll(value);
}

bool InstProto::is_valid() const {
    return id != -1;
}

InstProto::operator bool() const { return is_valid(); }

Instruction::Instruction(std::string_view mnemonic, const std::array<std::string, 3> &args) noexcept {
    auto proto = rv64::is::Rv64IMC::get_inst_proto(std::string(mnemonic));
    if (!proto) return;

    // parse arguments
    for (int i = 0; i < 3; ++i) {
        auto arg = args[i];
        m_proto_id = -i - 1; // invalid by default

        if (proto.args[i] == InstArgType::None) break;
        // register argument
        if (proto.args[i] == InstArgType::IntReg) {
            rv64::Reg reg(arg);

            if (!reg.is_valid()) return;
            this->m_args[i] = reg;
            continue;
        }
        // immediate argument
        if (proto.args[i] == InstArgType::Imm12) {
            if (!in_range<int12>(arg)) return;
            this->m_args[i] = int12(std::stoll(arg));
        } else if (proto.args[i] == InstArgType::Imm20) {
            if (!in_range<int20>(arg)) return;
            this->m_args[i] = int20(std::stoll(arg));
        } else if (proto.args[i] == InstArgType::UImm5) {
            if (!in_range<uint5>(arg)) return;
            this->m_args[i] = uint5(std::stoull(arg));
        } else if (proto.args[i] == InstArgType::UImm6) {
            if (!in_range<uint6>(arg)) return;
            this->m_args[i] = uint6(std::stoull(arg));
        } else if (proto.args[i] == InstArgType::UImm12) {
            if (!in_range<uint12>(arg)) return;
            this->m_args[i] = uint12(std::stoull(arg));
        } else
            assert(false && "unreachable");
    }
    m_proto_id = proto.id;
}

bool Instruction::is_valid() const { return m_proto_id > 0; }

Instruction::operator bool() const { return is_valid(); }

const std::array<InstArg, 3> &Instruction::get_args() const noexcept {
    return m_args;
}

InstProto Instruction::get_prototype() const noexcept {
    return rv64::is::Rv64IMC::get_inst_proto(m_proto_id);
}

const Instruction & Instruction::get_invalid_cref() noexcept {
    static Instruction invalid_inst{};
    return invalid_inst;
}
