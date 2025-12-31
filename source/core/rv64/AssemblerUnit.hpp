#pragma once
#include <Instruction.hpp>
#include <span>
#include <vector>
#include <bit>

#include "parser/asm_parsing.hpp"

namespace rv64 {
    class AssemblerUnit {
    public:
        static std::vector<uint8_t> assemble(std::span<const Instruction> insts,
                                             std::endian endian = std::endian::native);

        static std::vector<uint8_t> assemble(const asm_parsing::ParsedInstVec &insts,
                                             std::endian endian = std::endian::native);
    private:
        static std::variant<std::array<uint8_t, 2>, std::array<uint8_t, 4>>
        encode_instruction(const Instruction &inst, std::endian endian);
    };
}
