#pragma once
#include <array>
#include <variant>
#include "common.hpp"

namespace rv64 {
    class Cpu;
    class IntReg;
}

enum class InstArgType {
    None = 0,
    IntReg,
    Imm12,
    Imm20,
    UImm5,
    UImm6,
    UImm12,
    UImm20
};

using InstArg = std::variant<std::monostate, rv64::IntReg *, int12, int20, uint12, uint5, uint6>;


struct InstProto {
    const std::string_view name;
    const std::array<InstArgType, 3> args;
    const int id;

    explicit operator bool() const { return id != 0; }
};

static inline InstProto invalid_inst_proto{"", {InstArgType::None, InstArgType::None, InstArgType::None}, 0};

struct Instruction {
    int proto_id = 0;
    std::array<InstArg, 3> args;

    Instruction(rv64::Cpu &, std::string_view mnemonic, const std::array<std::string_view, 3> &args);

    explicit operator bool() const { return proto_id > 0; }
};

