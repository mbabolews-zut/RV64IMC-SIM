#include "asm_parsing.hpp"

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

            if (!builder.resolve_symbols(sym_map, current_pc)) {
                return 2;
            }

            auto inst = builder.build();
            if (!inst.is_valid()) {
                return 3;
            }

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