#pragma once
#include <parser/asm_parsing.hpp>
#include <vector>


class ParserProcessor {
public:

    ParserProcessor() {
        m_instructions.reserve(32);
    }
    void push_param(const std::string &str) {
        m_parms[m_parm_n % m_parms.size()] = str;
        m_parm_n++;
    }
    void push_instruction(const std::string &str, size_t line) {
        // TODO: integrate with a symbol table
        // TODO: pseudo instructions
        Instruction inst {str, m_parms};
        m_instructions.emplace_back(line, inst);
        if (tolower(str[0]) != 'c') {
            m_instructions.emplace_back(SIZE_MAX, Instruction::get_invalid_cref());
        }
        m_parm_n = 0;
    }

    asm_parsing::ParsedInstVec get_parsed_instructions() const {
        return m_instructions;
    }

    void reset() {
        m_parm_n = 0;
        m_instructions.clear();
        m_instructions.reserve(32);
    }

private:
    size_t m_parm_n = 0;
    std::array<std::string, 3> m_parms;
    asm_parsing::ParsedInstVec m_instructions; // line and instruction
};


