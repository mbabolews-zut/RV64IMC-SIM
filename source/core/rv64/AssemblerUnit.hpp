#pragma once
#include <Instruction.hpp>
#include <span>
#include <vector>
#include <bit>

namespace rv64 {
    class AssemblerUnit {
    public:
        static std::vector<uint8_t> assemble(std::span<const Instruction> insts,
                                             std::endian endian = std::endian::native);
    };
}