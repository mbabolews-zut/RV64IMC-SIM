#include "ParserProcessor.hpp"
#include <InstructionBuilder.hpp>
#include <ui.hpp>
#include <cassert>
#include <algorithm>
#include <unordered_set>

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
    if (auto err = m_sym_table.add_label(name, m_byte_offset)) {
        ui::print_error(err->format());
    }
}

void ParserProcessor::push_instruction(std::string str, size_t line) {
    str = to_lowercase(str);

    InstructionBuilder builder(str);

    static const std::unordered_set<std::string_view> branch_instr{
        "beq","bne","blt","bltu","bge","bgeu"
    };

    bool is_branch = branch_instr.contains(str);
    bool is_jal = (str == "jal");

    for (size_t i = 0; i < m_parm_n; ++i) {
        bool is_offset_arg = (is_branch && i == 2) || (is_jal && i == 1);
        if (is_offset_arg) {
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
