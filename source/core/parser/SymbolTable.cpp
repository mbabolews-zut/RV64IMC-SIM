#include "SymbolTable.hpp"
#include <format>

std::optional<BuildError> asm_parsing::SymbolTable::add_label(std::string_view name, uint64_t address) {
    auto str = std::string{name};
    if (m_symbols.contains(str)) {
        return BuildError{
            .kind = BuildErrorKind::DuplicateLabel,
            .message = std::format("Duplicate label '{}'", str)
        };
    }
    m_symbols[str] = Symbol{
        .type = Symbol::Type::Label,
        .name = str,
        .address = address + m_data_offset
    };
    return std::nullopt;
}

void asm_parsing::SymbolTable::clear() {
    m_symbols.clear();
}

asm_parsing::SymbolTable::~SymbolTable() {
    clear();
}

std::unordered_map<std::string, uint64_t> asm_parsing::SymbolTable::export_symbol_map() const {
    std::unordered_map<std::string, uint64_t> result;
    result.reserve(m_symbols.size());
    for (const auto &p : m_symbols) {
        result.emplace(p.first, p.second.address);
    }
    return result;
}
