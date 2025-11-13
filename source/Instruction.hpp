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

/// Represents an unresolved symbol (label reference) in the instruction
struct UnresolvedSymbol {
    std::string name;
    int64_t offset = 0; /// Optional offset from the symbol
};

/// Raw parsed argument before validation
using RawInstArg = std::variant<
    std::monostate,
    std::string,      /// Register name or immediate string or symbol
    int64_t,          /// Parsed immediate
    UnresolvedSymbol  /// Symbol that needs resolution
>;

/// Fully resolved and validated instruction argument
using InstArg = std::variant<
    std::monostate, rv64::Reg,
    int12, int20, uint12, uint5, uint6, uint20, uint8, int8, int11, int6, int5
>;

struct InstProto {
    const std::string_view mnemonic;
    const std::array<InstArgType, 3> args;
    const int id;

    /// @brief checks if the instruction prototype is valid (id != -1).
    [[nodiscard]] bool is_valid() const;

    /// @brief checks if the instruction prototype is valid (id != -1).
    explicit operator bool() const;

    /// @brief checks if this is a branch instruction that encodes PC-relative offsets
    [[nodiscard]] bool is_branch() const;

    [[nodiscard]] size_t byte_size() const noexcept;
};

static inline constexpr InstProto invalid_inst_proto{"", {InstArgType::None, InstArgType::None, InstArgType::None}, -1};


/// @brief Represents a CPU instruction with its mnemonic and arguments.
class Instruction {
public:
    /// @brief constructs an invalid instruction
    Instruction() = default;

    /// @brief checks if the instruction is valid.
    [[nodiscard]] bool is_valid() const;

    /// @brief checks if the instruction is valid.
    explicit operator bool() const;

    [[nodiscard]] const std::array<InstArg, 3> &get_args() const noexcept;

    [[nodiscard]] InstProto get_prototype() const noexcept;

    [[nodiscard]] size_t byte_size() const noexcept;

    static const Instruction& get_invalid_cref() noexcept;

private:
    friend class InstructionBuilder;

    /// @brief constructs a validated instruction (only accessible by builder)
    Instruction(int proto_id, const std::array<InstArg, 3> &args);

    int m_proto_id = -1;
    std::array<InstArg, 3> m_args;
};

