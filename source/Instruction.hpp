#pragma once
#include <array>
#include <variant>
#include "common.hpp"
#include "rv64/GPIntReg.hpp"
#include "rv64/Reg.hpp"

namespace rv64 {
    class Cpu;
    using GPIntRegRef = std::reference_wrapper<GPIntReg>;
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


using InstArg = std::variant<
    std::monostate, rv64::Reg,
    int12, int20, uint12, uint5, uint6
>;

struct InstProto {
    const std::string_view mnemonic;
    const std::array<InstArgType, 3> args;
    const int id;

    /// @brief checks if the instruction prototype is valid (id != -1).
    /// * useful if InstProto is obtained from a lookup function.
    [[nodiscard]] bool is_valid() const;

    /// @brief checks if the instruction prototype is valid (id != -1).
    /// * useful if InstProto is obtained from a lookup function.
    explicit operator bool() const;
};

static inline constexpr InstProto invalid_inst_proto{"", {InstArgType::None, InstArgType::None, InstArgType::None}, -1};

/// @brief Represents a CPU instruction with its mnemonic and arguments.
class Instruction {
public:
    /// @brief constructs an instruction from its mnemonic and arguments.
    /// @attention check validity with is_valid() or operator bool().
    /// @param mnemonic instruction mnemonic (e.g., "addi")
    /// @param args instruction arguments in string form. (e.g., {"x1", "x2", "0x10"})
    Instruction(std::string_view mnemonic, const std::array<std::string, 3> &args) noexcept;

    /// @brief checks if the instruction is valid.
    [[nodiscard]] bool is_valid() const;

    /// @brief checks if the instruction is valid.
    explicit operator bool() const;

    [[nodiscard]] const std::array<InstArg, 3> &get_args() const noexcept;

    [[nodiscard]] InstProto get_prototype() const noexcept;

    static const Instruction& get_invalid_cref() noexcept;

private:
    Instruction() = default;

    int m_proto_id = -1;
    std::array<InstArg, 3> m_args;
};

