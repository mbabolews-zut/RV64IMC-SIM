#pragma once
#include <cstdint>
#include <string>

namespace asm_parsing {
    struct Symbol {
        enum class Type {
            Label = 0,
        };

        Type type;
        std::string name;
        uint64_t address = 0;
    };
}
