#pragma once
#include <array>
#include <variant>
#include <string>
#include "common.hpp"
#include "rv64/GPIntReg.hpp"
#include "rv64/Reg.hpp"

namespace rv64 {
    class Cpu;
    using GPIntRegRef = std::reference_wrapper<GPIntReg>;
}

enum class InstArgType {
    None = 0,
    IntReg, ///< General purpose integer register (x0-x31)
    IntRegP, ///< Register from compressed set (x8-x15)
    Imm5,
    Imm6,
    Imm8,
    Imm11,
    Imm12,
    Imm20,
    UImm5,
    UImm6,
    UImm8,
    UImm12,
    UImm20,
};

struct UnresolvedSymbol {
    std::string name;
    int64_t offset = 0;
};

using RawInstArg = std::variant<
    std::monostate,
    std::string,
    int64_t,
    UnresolvedSymbol
>;

using InstArg = std::variant<
    std::monostate, rv64::Reg,
    int12, int20, uint12, uint5, uint6, uint20, uint8, int8, int11, int6, int5
>;

struct InstProto {
    const std::string_view mnemonic;
    const std::array<InstArgType, 3> args;
    const int id;

    [[nodiscard]] bool is_valid() const;
    explicit operator bool() const;
    [[nodiscard]] bool is_branch() const;
    [[nodiscard]] size_t byte_size() const noexcept;
};

static inline constexpr InstProto invalid_inst_proto{"", {InstArgType::None, InstArgType::None, InstArgType::None}, -1};

class Instruction {
public:
    Instruction() = default;
    Instruction(const Instruction &) = default;
    Instruction(Instruction &&) noexcept = default;
    Instruction &operator=(const Instruction &) = default;
    Instruction &operator=(Instruction &&) noexcept = default;

    [[nodiscard]] bool is_valid() const;
    explicit operator bool() const;

    [[nodiscard]] const std::array<InstArg, 3> &get_args() const noexcept;
    [[nodiscard]] InstProto get_prototype() const noexcept;
    [[nodiscard]] size_t byte_size() const noexcept;

    [[nodiscard]] static const Instruction& invalid_cref() noexcept;
    [[nodiscard]] static Instruction invalid() noexcept { return invalid_cref(); }

    [[nodiscard]] bool is_padding() const noexcept { return !is_valid(); }

private:
    friend class InstructionBuilder;
    Instruction(int proto_id, const std::array<InstArg, 3> &args);

    int m_proto_id = -1;
    std::array<InstArg, 3> m_args{};
};
