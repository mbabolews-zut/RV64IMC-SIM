#pragma once
#include <climits>
#include <string>
#include <vector>

#include "Instruction.hpp"

class ParserProcessor;
namespace asm_parsing {

    struct ParsedInst {
        size_t lineno = SIZE_MAX;
        Instruction inst;

        [[nodiscard]] bool is_padding() const {
            return lineno == SIZE_MAX;
        }
    };

    using ParsedInstVec = std::vector<ParsedInst>;

    int parse(ParsedInstVec &result, const std::string &str);
}