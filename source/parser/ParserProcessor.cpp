#include "ParserProcessor.hpp"
#include <InstructionBuilder.hpp>
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

    str = to_lowercase(str);

    // Build instruction using the builder pattern
    InstructionBuilder builder(str);

    // For branch instructions (except jalr), divide the immediate by 2
    // This is because branch offsets are encoded as multiples of 2 bytes
    static constexpr std::array<std::string_view, 7> branch_instr{
        "beq","bne","blt","bltu","bge","bgeu","jal"
    };

    bool is_branch = std::ranges::find(branch_instr, str) != branch_instr.end();

    for (size_t i = 0; i < m_parm_n; ++i) {
        // Special handling for branch instruction immediates
        if (is_branch && i == 2) {
            try {
                int64_t imm = std::stoll(m_parms[i]);
                builder.add_imm(imm / 2);
            } catch (...) {
                // If parsing fails, it might be a symbol - add as-is and let symbol resolution handle it
                builder.add_arg(m_parms[i]);
            }
        } else {
            builder.add_arg(m_parms[i]);
        }
    }

    Instruction inst = builder.build();
    m_instructions.emplace_back(line, inst);

    // Add padding for non-compressed instructions (4-byte alignment)
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
