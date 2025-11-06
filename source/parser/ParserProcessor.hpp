#pragma once
#include <Instruction.hpp>
#include <vector>

class ParserProcessor;
int asm_parse(ParserProcessor &pproc, const std::string &str);

class ParserProcessor {
public:
    using ParsedInstVec = std::vector<std::pair<size_t, Instruction>>; // pair (line, instruction)

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
        m_parm_n = 0;
    }

    ParsedInstVec get_parsed_instructions() {
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
    ParsedInstVec m_instructions; // line and instruction
};


