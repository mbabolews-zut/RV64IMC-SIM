#pragma once
#include <parser/asm_parsing.hpp>
#include <array>

class ParserProcessor {
public:

    ParserProcessor();
    void push_param(const std::string &str);
    void push_instruction(std::string str, size_t line);

    [[nodiscard]] asm_parsing::ParsedInstVec get_parsed_instructions() const;

    void reset();

private:
    size_t m_parm_n = 0;
    std::array<std::string, 3> m_parms;
    asm_parsing::ParsedInstVec m_instructions; // line and instruction
};


