#include "asm_parsing.hpp"
#include <ui.hpp>

namespace asm_parsing {
    int ParsingResult::resolve_instructions(ParsedInstVec &out_instructions, uint64_t data_off) const {
        auto sym_map = symbol_table.export_symbol_map();

        ParsedInstVec result;
        result.reserve(unresolved_instructions.size());

        uint64_t current_pc = data_off;

        for (const auto &uinst: unresolved_instructions) {
            if (uinst.lineno == SIZE_MAX) {
                result.push_back(ParsedInst{SIZE_MAX, Instruction::invalid_cref()});
                continue;
            }

            InstructionBuilder builder = uinst.builder;

            if (auto err = builder.resolve_symbols(sym_map, current_pc)) {
                err->line = uinst.lineno;
                ui::print_error(err->format());
                return 2;
            }

            auto build_result = builder.build();
            if (auto *err = std::get_if<BuildError>(&build_result)) {
                err->line = uinst.lineno;
                ui::print_error(err->format());
                return 3;
            }

            auto &inst = std::get<Instruction>(build_result);
            result.push_back(ParsedInst{uinst.lineno, inst});
            current_pc += inst.byte_size();
        }

        out_instructions.insert(out_instructions.end(), result.begin(), result.end());
        return 0;
    }

    int parse_and_resolve(const std::string &source, ParsedInstVec &out_instructions, uint64_t data_offset) {
        auto result = parse(source);
        if (result.error_code != 0)
            return 1;

        return result.resolve_instructions(out_instructions, data_offset);
    }
}