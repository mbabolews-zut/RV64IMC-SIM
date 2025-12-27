#pragma once
#include <Instruction.hpp>
#include <unordered_map>

/// @brief Builder for constructing instructions in stages
class InstructionBuilder {
public:
    InstructionBuilder() = default;
    explicit InstructionBuilder(std::string_view mnemonic);

    InstructionBuilder& set_mnemonic(std::string_view mnemonic);

    /// @brief adds a raw argument (register name, immediate string, or symbol)
    InstructionBuilder& add_arg(std::string_view arg);

    /// @brief adds a parsed immediate argument
    InstructionBuilder& add_imm(int64_t value);

    /// @brief adds a symbol reference
    InstructionBuilder& add_symbol(std::string_view symbol, int64_t offset = 0);

    /// @brief resolves symbols using the provided symbol table
    /// @param symbol_table Map of symbol names to absolute addresses
    /// @param current_pc Current instruction's PC (for branch offset calculation)
    /// @return true if all symbols were resolved
    bool resolve_symbols(const std::unordered_map<std::string, uint64_t> &symbol_table, uint64_t current_pc = 0);

    /// @brief validates and builds the final instruction
    /// @return valid Instruction or invalid instruction if validation fails
    [[nodiscard]] Instruction build() const;

    void reset();

private:
    std::string m_mnemonic;
    std::array<RawInstArg, 3> m_raw_args;
    size_t m_arg_count = 0;
};