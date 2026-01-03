#pragma once
#include <parser/Symbol.hpp>
#include <BuildError.hpp>
#include <unordered_map>
#include <optional>

namespace asm_parsing {
    class SymbolTable {
    public:
        SymbolTable() : m_data_offset(0) {}
        explicit SymbolTable(uint64_t data_offset) : m_data_offset(data_offset) {}
        ~SymbolTable();

        /// @return nullopt on success, BuildError on duplicate label
        [[nodiscard]] std::optional<BuildError> add_label(std::string_view name, uint64_t address);

        void clear();

        [[nodiscard]] std::unordered_map<std::string, uint64_t> export_symbol_map() const;

    private:
        uint64_t m_data_offset = 0;
        std::unordered_map<std::string, Symbol> m_symbols;
    };
}
