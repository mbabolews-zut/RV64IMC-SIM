#pragma once
#include <parser/asm_parsing.hpp>
#include <parser/SymbolTable.hpp>
#include <InstructionBuilder.hpp>
#include <array>

class ParserProcessor {
public:

    ParserProcessor();
    void push_param(const std::string &str);
    void push_instruction(std::string str, size_t line);
    void add_label(std::string_view name);

    [[nodiscard]] asm_parsing::ParsedInstVec get_parsed_instructions() const;

    [[nodiscard]] asm_parsing::ParsingResult get_parsing_result() const;

    void reset();

private:
    size_t m_parm_n = 0;
    std::array<std::string, 3> m_parms;

    size_t m_inst_bytes = 0;
    uint64_t m_byte_offset = 0;

    std::vector<asm_parsing::InstUnderConstruction> m_inst_builders; // line and instruction
    asm_parsing::SymbolTable m_sym_table;
};
