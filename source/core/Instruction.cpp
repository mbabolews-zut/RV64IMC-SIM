#include "Instruction.hpp"
#include <rv64/instruction_sets/Rv64IMC.hpp>

#include "rv64/Cpu.hpp"


// InstProto methods
bool InstProto::is_valid() const {
    return id != -1;
}

InstProto::operator bool() const {
    return is_valid();
}

bool InstProto::is_branch() const {
    return std::ranges::any_of(
        std::array{"beq"sv, "bne"sv, "blt"sv, "bltu"sv, "bge"sv, "bgeu"sv, "jal"sv},
        [&](auto b) { return mnemonic == b; }
        );
}

size_t InstProto::byte_size() const noexcept {
    if (!is_valid()) return 0;
    if (mnemonic.size() >= 2 && mnemonic[0] == 'c' && mnemonic[1] == '.') {
        return 2; // compressed instruction
    }
    return 4; // standard instruction size
}

// Instruction methods
Instruction::Instruction(int proto_id, const std::array<InstArg, 3> &args)
    : m_proto_id(proto_id), m_args(args) {
}

bool Instruction::is_valid() const {
    return m_proto_id >= 0;
}

Instruction::operator bool() const {
    return is_valid();
}

const std::array<InstArg, 3> &Instruction::get_args() const noexcept {
    return m_args;
}

InstProto Instruction::get_prototype() const noexcept {
    return rv64::is::Rv64IMC::get_inst_proto(m_proto_id);
}

size_t Instruction::byte_size() const noexcept {
    if (m_proto_id == -1) return 0;
    return get_prototype().byte_size();
}

const Instruction &Instruction::invalid_cref() noexcept {
    static Instruction invalid_inst{};
    return invalid_inst;
}
