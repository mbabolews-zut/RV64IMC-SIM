#pragma once
#include <array>
#include <variant>
#include "common.hpp"

namespace rv64 {
    class VM;
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

using InstArg = std::variant<std::monostate, rv64::IntReg*, int12, int20, uint12, uint5, uint6>;

struct Instruction {
    const std::string_view name;
    const std::array<InstArgType, 3> args;
};
