#include "ParserProcessor.hpp"
#include <InstructionBuilder.hpp>
#include <cassert>
#include <algorithm>

ParserProcessor::ParserProcessor() : m_sym_table(0) {
    m_inst_builders.reserve(32);
}

void ParserProcessor::push_param(const std::string &str) {
    if (m_parm_n >= m_parms.size()) {
        return;
    }
    m_parms[m_parm_n] = str;
    m_parm_n++;
}

void ParserProcessor::add_label(std::string_view name) {
    (void)m_sym_table.add_label(name, m_byte_offset);
}

void ParserProcessor::push_instruction(std::string str, size_t line) {
    str = to_lowercase(str);

    InstructionBuilder builder(str);

    static constexpr std::array<std::string_view, 7> branch_instr{
        "beq","bne","blt","bltu","bge","bgeu","jal"
    };

    bool is_branch = std::ranges::find(branch_instr, str) != branch_instr.end();

    for (size_t i = 0; i < m_parm_n; ++i) {
        if (is_branch && i == 2) {
            try {
                int64_t imm = std::stoll(m_parms[i]);
                builder.add_imm(imm / 2);
            } catch (...) {
                builder.add_symbol(m_parms[i], 0);
            }
        } else {
            builder.add_arg(m_parms[i]);
        }
    }

    m_inst_builders.emplace_back(asm_parsing::InstUnderConstruction{line, std::move(builder)});

    if (str[0] != 'c' || (str.size() > 1 && str[1] != '.')) {
        m_inst_builders.emplace_back(asm_parsing::InstUnderConstruction{SIZE_MAX, InstructionBuilder()});
        m_byte_offset += 4;
    } else {
        m_byte_offset += 2;
    }

    m_parm_n = 0;
}

asm_parsing::ParsedInstVec ParserProcessor::get_parsed_instructions() const {
    asm_parsing::ParsedInstVec out;
    asm_parsing::ParsingResult pr{m_inst_builders, m_sym_table};
    int rv = pr.resolve_instructions(out, 0);
    (void)rv; // TODO: handle errors
    return out;
}

asm_parsing::ParsingResult ParserProcessor::get_parsing_result() const {
    return asm_parsing::ParsingResult{m_inst_builders, m_sym_table};
}

void ParserProcessor::reset() {
    m_parm_n = 0;
    m_inst_bytes = 0;
    m_inst_builders.clear();
    m_inst_builders.reserve(32);
    m_sym_table.clear();
    m_byte_offset = 0;
    for (auto &p : m_parms) p.clear();
}
