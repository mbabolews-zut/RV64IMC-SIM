#pragma once
#include <climits>
#include <string>
#include <vector>

#include "Instruction.hpp"
#include "InstructionBuilder.hpp"
#include "SymbolTable.hpp"

class ParserProcessor;
namespace asm_parsing {

    struct ParsedInst {
        size_t lineno = SIZE_MAX;
        Instruction inst;

        [[nodiscard]] bool is_padding() const {
            return lineno == SIZE_MAX;
        }
    };

    struct InstUnderConstruction {
        size_t lineno = SIZE_MAX;
        InstructionBuilder builder;
    };

    using ParsedInstVec = std::vector<ParsedInst>;
    using InstBuilderVec = std::vector<InstUnderConstruction>;

    struct ParsingResult {
        InstBuilderVec unresolved_instructions;
        SymbolTable symbol_table;

        int error_code = 0;
        [[nodiscard]] int resolve_instructions(ParsedInstVec &out_instructions, uint64_t data_off = 0) const;
    };

    /// @brief Parse assembly source code into unresolved instructions
    /// @param source Assembly source code
    /// @return ParsingResult containing unresolved instructions and symbol table
    [[nodiscard]] ParsingResult parse(const std::string &source);

    /// @brief Parse and resolve assembly source code into executable instructions
    /// @param source Assembly source code
    /// @param out_instructions Output vector for resolved instructions
    /// @param data_offset
    /// @return 0 on success, 1 on parse error, 2 on symbol resolution error, 3 on validation error
    [[nodiscard]] int parse_and_resolve(const std::string &source, ParsedInstVec &out_instructions, uint64_t data_offset);
}