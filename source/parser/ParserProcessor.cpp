#include "ParserProcessor.hpp"
#include <cassert>
#include <algorithm>

ParserProcessor::ParserProcessor() {
    m_instructions.reserve(32);
}

void ParserProcessor::push_param(const std::string &str) {
    assert(m_parm_n <= m_parms.size());
    m_parms[m_parm_n] = str;
    m_parm_n++;
}

void ParserProcessor::push_instruction(std::string str, size_t line) {
    assert(m_parm_n <= m_parms.size());
    // TODO: integrate with a symbol table
    // TODO: pseudo instructions
    str = to_lowercase(str);

    // parser param modifications
    // for branch instructions (except jalr), divide the immediate by 2
    static constexpr std::array<std::string_view, 7> branch_instr{
        "beq","bne","blt","bltu","bge","bgeu","jal"
    };

    if (std::ranges::find(branch_instr, str) != branch_instr.end()) {
        m_parms[2] = std::to_string(std::stoll(m_parms[2]) / 2);
    }

    Instruction inst {str, m_parms};
    m_instructions.emplace_back(line, inst);
    if (tolower(str[0]) != 'c') {
        m_instructions.emplace_back(SIZE_MAX, Instruction::get_invalid_cref());
    }
    m_parm_n = 0;
}

asm_parsing::ParsedInstVec ParserProcessor::get_parsed_instructions() const {
    return m_instructions;
}

void ParserProcessor::reset() {
    m_parm_n = 0;
    m_instructions.clear();
    m_instructions.reserve(32);
}
