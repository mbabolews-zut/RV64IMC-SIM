#include "AssemblerUnit.hpp"

#include <cassert>
#include <cstring>
#include <numeric>
#include <endianness.hpp>
#include <unordered_map>

/// Custom I-format variants for instruction encoding
/// * Shift - funct6[31:26], imm[25:20], rs1[19:15], funct3[14:12], rd[11:7], opcode[6:0]
///   for shift immediate instructions (slli, srli, srai)
/// * ShiftW - funct7[31:25], imm[24:20], rs1[19:15], funct3[14:12], rd[11:7], opcode[6:0]
///   for shift immediate word instructions (slliw, srliw, sraiw)
enum class IFormat { R, I, S, B, U, J, CR, CI, CSS, CIW, CL, CS, CA, CB, CJ, Shift, ShiftW };

struct IEncoding {
    IFormat format;
    uint8_t opcode;
    uint16_t funct0; // first funct field from most significant bit (funct3, funct4, funct6, etc.)
    uint16_t funct1; // second funct field (typically funct3) (if applicable)
};

// clang-format off
std::unordered_map<std::string_view, IEncoding> enc_map {
    //=== (RV64I) Base Integer Instructions ===
    {"addi",  {.format = IFormat::I, .opcode = 0b0010011, .funct0 = 0b000}},
    {"slti",  {.format = IFormat::I, .opcode = 0b0010011, .funct0 = 0b010}},
    {"sltiu", {.format = IFormat::I, .opcode = 0b0010011, .funct0 = 0b011}},
    {"andi",  {.format = IFormat::I, .opcode = 0b0010011, .funct0 = 0b111}},
    {"ori",   {.format = IFormat::I, .opcode = 0b0010011, .funct0 = 0b110}},
    {"xori",  {.format = IFormat::I, .opcode = 0b0010011, .funct0 = 0b100}},


    {"slli",  {.format = IFormat::Shift, .opcode = 0b0010011, .funct0 = 0b000000, .funct1 = 0b001}},
    {"srli",  {.format = IFormat::Shift, .opcode = 0b0010011, .funct0 = 0b000000, .funct1 = 0b101}},
    {"srai",  {.format = IFormat::Shift, .opcode = 0b0010011, .funct0 = 0b010000, .funct1 = 0b101}},

    {"lui",   {.format = IFormat::U, .opcode = 0b0110111}},
    {"auipc", {.format = IFormat::U, .opcode = 0b0010111}},

    {"add",  {.format = IFormat::R, .opcode = 0b0110011, .funct0 = 0b0000000, .funct1 = 0b000}},
    {"sub",  {.format = IFormat::R, .opcode = 0b0110011, .funct0 = 0b0100000, .funct1 = 0b000}},
    {"slt",  {.format = IFormat::R, .opcode = 0b0110011, .funct0 = 0b0000000, .funct1 = 0b010}},
    {"sltu", {.format = IFormat::R, .opcode = 0b0110011, .funct0 = 0b0000000, .funct1 = 0b011}},
    {"and",  {.format = IFormat::R, .opcode = 0b0110011, .funct0 = 0b0000000, .funct1 = 0b111}},
    {"or",   {.format = IFormat::R, .opcode = 0b0110011, .funct0 = 0b0000000, .funct1 = 0b110}},
    {"xor",  {.format = IFormat::R, .opcode = 0b0110011, .funct0 = 0b0000000, .funct1 = 0b100}},
    {"sll",  {.format = IFormat::R, .opcode = 0b0110011, .funct0 = 0b0000000, .funct1 = 0b001}},
    {"srl",  {.format = IFormat::R, .opcode = 0b0110011, .funct0 = 0b0000000, .funct1 = 0b101}},
    {"sra",  {.format = IFormat::R, .opcode = 0b0110011, .funct0 = 0b0100000, .funct1 = 0b101}},
    {"jal",  {.format = IFormat::J, .opcode = 0b1101111}},
    {"jalr", {.format = IFormat::I, .opcode = 0b1100111, .funct0 = 0b000}},
    {"beq",  {.format = IFormat::B, .opcode = 0b1100011, .funct0 = 0b000}},
    {"bne",  {.format = IFormat::B, .opcode = 0b1100011, .funct0 = 0b001}},
    {"blt",  {.format = IFormat::B, .opcode = 0b1100011, .funct0 = 0b100}},
    {"bge",  {.format = IFormat::B, .opcode = 0b1100011, .funct0 = 0b101}},
    {"bltu", {.format = IFormat::B, .opcode = 0b1100011, .funct0 = 0b110}},
    {"bgeu", {.format = IFormat::B, .opcode = 0b1100011, .funct0 = 0b111}},

    {"lb",  {.format = IFormat::I, .opcode = 0b0000011, .funct0 = 0b000}},
    {"lh",  {.format = IFormat::I, .opcode = 0b0000011, .funct0 = 0b001}},
    {"lw",  {.format = IFormat::I, .opcode = 0b0000011, .funct0 = 0b010}},
    {"ld",  {.format = IFormat::I, .opcode = 0b0000011, .funct0 = 0b011}},
    {"lbu", {.format = IFormat::I, .opcode = 0b0000011, .funct0 = 0b100}},
    {"lhu", {.format = IFormat::I, .opcode = 0b0000011, .funct0 = 0b101}},
    {"lwu", {.format = IFormat::I, .opcode = 0b0000011, .funct0 = 0b110}},

    {"sb", {.format = IFormat::S, .opcode = 0b0100011, .funct0 = 0b000}},
    {"sh", {.format = IFormat::S, .opcode = 0b0100011, .funct0 = 0b001}},
    {"sw", {.format = IFormat::S, .opcode = 0b0100011, .funct0 = 0b010}},
    {"sd", {.format = IFormat::S, .opcode = 0b0100011, .funct0 = 0b011}},

    {"addiw", {.format = IFormat::I, .opcode = 0b0011011, .funct0 = 0b000}},
    {"slliw", {.format = IFormat::ShiftW, .opcode = 0b0011011, .funct0 = 0b0000000, .funct1 = 0b001}},
    {"srliw", {.format = IFormat::ShiftW, .opcode = 0b0011011, .funct0 = 0b0000000, .funct1 = 0b101}},
    {"sraiw", {.format = IFormat::ShiftW, .opcode = 0b0011011, .funct0 = 0b0100000, .funct1 = 0b101}},

    {"addw", {.format = IFormat::R, .opcode = 0b0111011, .funct0 = 0b0000000, .funct1 = 0b000}},
    {"subw", {.format = IFormat::R, .opcode = 0b0111011, .funct0 = 0b0100000, .funct1 = 0b000}},
    {"sllw", {.format = IFormat::R, .opcode = 0b0111011, .funct0 = 0b0000000, .funct1 = 0b001}},
    {"srlw", {.format = IFormat::R, .opcode = 0b0111011, .funct0 = 0b0000000, .funct1 = 0b101}},
    {"sraw", {.format = IFormat::R, .opcode = 0b0111011, .funct0 = 0b0100000, .funct1 = 0b101}},

    // === M Extension ===
    {"mul",    {.format = IFormat::R, .opcode = 0b0110011, .funct0 = 0b0000001, .funct1 = 0b000}},
    {"mulh",   {.format = IFormat::R, .opcode = 0b0110011, .funct0 = 0b0000001, .funct1 = 0b001}},
    {"mulhsu", {.format = IFormat::R, .opcode = 0b0110011, .funct0 = 0b0000001, .funct1 = 0b010}},
    {"mulhu",  {.format = IFormat::R, .opcode = 0b0110011, .funct0 = 0b0000001, .funct1 = 0b011}},
    {"div",    {.format = IFormat::R, .opcode = 0b0110011, .funct0 = 0b0000001, .funct1 = 0b100}},
    {"divu",   {.format = IFormat::R, .opcode = 0b0110011, .funct0 = 0b0000001, .funct1 = 0b101}},
    {"rem",    {.format = IFormat::R, .opcode = 0b0110011, .funct0 = 0b0000001, .funct1 = 0b110}},
    {"remu",   {.format = IFormat::R, .opcode = 0b0110011, .funct0 = 0b0000001, .funct1 = 0b111}},

    {"mulw",  {.format = IFormat::R, .opcode = 0b0111011, .funct0 = 0b0000001, .funct1 = 0b000}},
    {"divw",  {.format = IFormat::R, .opcode = 0b0111011, .funct0 = 0b0000001, .funct1 = 0b100}},
    {"divuw", {.format = IFormat::R, .opcode = 0b0111011, .funct0 = 0b0000001, .funct1 = 0b101}},
    {"remw",  {.format = IFormat::R, .opcode = 0b0111011, .funct0 = 0b0000001, .funct1 = 0b110}},
    {"remuw", {.format = IFormat::R, .opcode = 0b0111011, .funct0 = 0b0000001, .funct1 = 0b111}},

    // === C Extension ===
    {"c.addi4spn", {.format = IFormat::CIW, .opcode = 0b00, .funct0 = 0b000}},
    {"c.lw",       {.format = IFormat::CL,  .opcode = 0b00, .funct0 = 0b010}},
    {"c.ld",       {.format = IFormat::CL,  .opcode = 0b00, .funct0 = 0b011}},
    {"c.sw",       {.format = IFormat::CS,  .opcode = 0b00, .funct0 = 0b110}},
    {"c.sd",       {.format = IFormat::CS,  .opcode = 0b00, .funct0 = 0b111}},

    {"c.addi",     {.format = IFormat::CI, .opcode = 0b01, .funct0 = 0b000}},
    {"c.addiw",    {.format = IFormat::CI, .opcode = 0b01, .funct0 = 0b001}},
    {"c.li",       {.format = IFormat::CI, .opcode = 0b01, .funct0 = 0b010}},
    {"c.addi16sp", {.format = IFormat::CI, .opcode = 0b01, .funct0 = 0b011}}, // rd=2
    {"c.lui",      {.format = IFormat::CI, .opcode = 0b01, .funct0 = 0b011}}, // rd!=2
    {"c.srli",     {.format = IFormat::CB, .opcode = 0b01, .funct0 = 0b100, .funct1 = 0b00}},
    {"c.srai",     {.format = IFormat::CB, .opcode = 0b01, .funct0 = 0b100, .funct1 = 0b01}},
    {"c.andi",     {.format = IFormat::CB, .opcode = 0b01, .funct0 = 0b100, .funct1 = 0b10}},
    {"c.sub",      {.format = IFormat::CA, .opcode = 0b01, .funct0 = 0b100011, .funct1 = 0b00}},
    {"c.xor",      {.format = IFormat::CA, .opcode = 0b01, .funct0 = 0b100011, .funct1 = 0b01}},
    {"c.or",       {.format = IFormat::CA, .opcode = 0b01, .funct0 = 0b100011, .funct1 = 0b10}},
    {"c.and",      {.format = IFormat::CA, .opcode = 0b01, .funct0 = 0b100011, .funct1 = 0b11}},
    {"c.subw",     {.format = IFormat::CA, .opcode = 0b01, .funct0 = 0b100111, .funct1 = 0b00}},
    {"c.addw",     {.format = IFormat::CA, .opcode = 0b01, .funct0 = 0b100111, .funct1 = 0b01}},
    {"c.j",        {.format = IFormat::CJ, .opcode = 0b01, .funct0 = 0b101}},
    {"c.beqz",     {.format = IFormat::CB, .opcode = 0b01, .funct0 = 0b110}},
    {"c.bnez",     {.format = IFormat::CB, .opcode = 0b01, .funct0 = 0b111}},

    {"c.slli",  {.format = IFormat::CI,  .opcode = 0b10, .funct0 = 0b000}},
    {"c.lwsp",  {.format = IFormat::CI,  .opcode = 0b10, .funct0 = 0b010}},
    {"c.ldsp",  {.format = IFormat::CI,  .opcode = 0b10, .funct0 = 0b011}},
    {"c.jr",    {.format = IFormat::CR,  .opcode = 0b10, .funct0 = 0b1000}},  // rs2=0
    {"c.mv",    {.format = IFormat::CR,  .opcode = 0b10, .funct0 = 0b1000}},  // rs2!=0
    {"c.ebreak",{.format = IFormat::CR,  .opcode = 0b10, .funct0 = 0b1001}},  // rd=0,rs2=0
    {"c.jalr",  {.format = IFormat::CR,  .opcode = 0b10, .funct0 = 0b1001}},  // rs2=0
    {"c.add",   {.format = IFormat::CR,  .opcode = 0b10, .funct0 = 0b1001}},  // rs2!=0
    {"c.swsp",  {.format = IFormat::CSS, .opcode = 0b10, .funct0 = 0b110}},
    {"c.sdsp",  {.format = IFormat::CSS, .opcode = 0b10, .funct0 = 0b111}},

    {"ecall",  {.format = IFormat::I, .opcode = 0b1110011, .funct0 = 0b000}}, // imm=0
    {"ebreak", {.format = IFormat::I, .opcode = 0b1110011, .funct0 = 0b000}}, // imm=1

    {"fence",   {.format = IFormat::I, .opcode = 0b0001111, .funct0 = 0b000}},
    {"fence.i", {.format = IFormat::I, .opcode = 0b0001111, .funct0 = 0b001}},
};
// clang-format on

uint32_t imm6_to_u32(const InstArg &imm6) {
    uint32_t result = 0;
    std::visit([&]<typename T>(T &&arg) {
        using U = std::decay_t<T>;
        if constexpr (std::is_same_v<U, int6> || std::is_same_v<U, uint6>)
            result = static_cast<uint32_t>(arg);
    }, imm6);
    return result;
}

namespace rv64 {
    std::vector<uint8_t> AssemblerUnit::assemble(std::span<const Instruction> insts, std::endian endian) {
        size_t bytecode_size = std::transform_reduce(
            insts.begin(), insts.end(), size_t{0}, std::plus{},
            [](const auto &inst) { return inst.byte_size(); }
        );

        std::vector<uint8_t> bytecode(bytecode_size);
        size_t offset = 0;

        for (const auto &inst: insts) {
            auto encoded = encode_instruction(inst, endian);
            std::visit([&](const auto &data) {
                std::memcpy(&bytecode[offset], data.data(), data.size());
                offset += data.size();
            }, encoded);
        }

        return bytecode;
    }

    std::vector<uint8_t> AssemblerUnit::assemble(const asm_parsing::ParsedInstVec &insts, std::endian endian) {
        std::vector<uint8_t> bytecode;

        for (const auto &parsed: insts) {
            if (parsed.is_padding()) continue;

            auto encoded = encode_instruction(parsed.inst, endian);
            std::visit([&](const auto &data) {
                bytecode.insert(bytecode.end(), data.begin(), data.end());
            }, encoded);
        }

        return bytecode;
    }

    std::variant<std::array<uint8_t, 2>, std::array<uint8_t, 4> >
    AssemblerUnit::encode_instruction(const Instruction &inst, std::endian endian) {
        if (!inst.is_valid()) {
            throw std::runtime_error("Invalid assembler instruction");
        }

        size_t size = inst.byte_size();
        std::string mnemonic = std::string(inst.get_prototype().mnemonic);

        // NOP instruction converting
        Instruction instruction = inst;
        if (mnemonic == "nop") {
            instruction = Instruction::create("addi", {Reg(0), Reg(0), int12(0)});
        } else if (mnemonic == "c.nop") {
            instruction = Instruction::create("c.addi", {Reg(0), int6(0)});
        } else {
            instruction = inst;
        }
        mnemonic = std::string(instruction.get_prototype().mnemonic);


        const auto &args = instruction.get_args();

        assert(enc_map.contains(mnemonic));
        auto [format, opcode, functHi, functLo] = enc_map[mnemonic];

        uint32_t encoded = opcode;

        std::array<int, 3> r{};
        for (size_t i = 0; i < 3; ++i) {
            if (std::holds_alternative<Reg>(args[i])) {
                r[i] = std::get<Reg>(args[i]).idx();
            }
        }

        switch (format) {
            case IFormat::R:
                encoded |= r[0] << 7;
                encoded |= functLo << 12;
                encoded |= r[1] << 15;
                encoded |= r[2] << 20;
                encoded |= functHi << 25;
                break;

            case IFormat::I: {
                encoded |= r[0] << 7;
                encoded |= functHi << 12;
                encoded |= r[1] << 15;
                if (mnemonic == "ecall") encoded |= 0 << 20;
                else if (mnemonic == "ebreak") encoded |= 1 << 20;
                else if (std::holds_alternative<uint12>(args[2]))
                    encoded |= std::get<uint12>(args[2]) << 20;
                else encoded |= std::get<int12>(args[2]) << 20;
                break;
            }
            case IFormat::S: {
                uint32_t imm = std::get<int12>(args[2]);
                encoded |= (imm & 0x1F) << 7;
                encoded |= functHi << 12;
                encoded |= r[1] << 15;
                encoded |= r[0] << 20;
                encoded |= (imm >> 5) << 25;
                break;
            }
            case IFormat::B: {
                // imm was divided by 2 in resolve_symbols, multiply back to get byte offset
                int32_t imm = std::get<int12>(args[2]) * 2;
                encoded |= ((imm >> 12) & 0x1) << 31; // imm[12]
                encoded |= ((imm >> 5) & 0x3F) << 25; // imm[10:5]
                encoded |= ((imm >> 1) & 0xF) << 8; // imm[4:1]
                encoded |= ((imm >> 11) & 0x1) << 7; // imm[11]
                encoded |= functHi << 12;
                encoded |= r[0] << 15;
                encoded |= r[1] << 20;
                break;
            }
            case IFormat::U: {
                uint32_t imm = std::get<int20>(args[1]);
                encoded |= r[0] << 7;
                encoded |= imm << 12;
                break;
            }
            case IFormat::J: {
                // imm was divided by 2 in resolve_symbols, multiply back to get byte offset
                int32_t imm = std::get<int20>(args[1]) * 2;
                encoded |= ((imm >> 20) & 0x1) << 31; // imm[20]
                encoded |= ((imm >> 1) & 0x3FF) << 21; // imm[10:1]
                encoded |= ((imm >> 11) & 0x1) << 20; // imm[11]
                encoded |= ((imm >> 12) & 0xFF) << 12; // imm[19:12]
                encoded |= r[0] << 7;
                break;
            }
            case IFormat::Shift: {
                uint32_t shamt = std::get<uint6>(args[2]);
                encoded |= r[0] << 7;
                encoded |= functLo << 12;
                encoded |= r[1] << 15;
                encoded |= shamt << 20;
                encoded |= functHi << 26;
                break;
            }
            case IFormat::ShiftW: {
                uint32_t shamt = std::get<uint5>(args[2]);
                encoded |= r[0] << 7;
                encoded |= functLo << 12;
                encoded |= r[1] << 15;
                encoded |= shamt << 20;
                encoded |= functHi << 25;
                break;
            }
            case IFormat::CR:
                encoded |= r[1] << 2;
                encoded |= r[0] << 7;
                encoded |= functHi << 12;
                break;

            case IFormat::CI: {
                uint32_t imm = imm6_to_u32(args[1]);
                encoded |= (imm & 0x1F) << 2;
                encoded |= r[0] << 7;
                encoded |= ((imm >> 5) & 0x1) << 12;
                encoded |= functHi << 13;
                break;
            }
            case IFormat::CSS: {
                uint32_t imm = imm6_to_u32(args[1]);
                encoded |= r[0] << 2;
                encoded |= (imm & 0x3F) << 7;
                encoded |= functHi << 13;
                break;
            }
            case IFormat::CIW: {
                uint32_t imm = std::get<uint8>(args[1]);
                uint32_t rd_prime = r[0] - 8;
                encoded |= rd_prime << 2;
                encoded |= (imm & 0xFF) << 5;
                encoded |= functHi << 13;
                break;
            }
            case IFormat::CL: {
                uint32_t imm = std::get<uint5>(args[2]);
                uint32_t rs1_prime = r[1] - 8;
                uint32_t rd_prime = r[0] - 8;
                encoded |= rd_prime << 2;
                encoded |= ((imm >> 3) & 0x3) << 5;
                encoded |= rs1_prime << 7;
                encoded |= (imm & 0x7) << 10;
                encoded |= functHi << 13;
                break;
            }
            case IFormat::CS: {
                uint32_t imm = std::get<uint5>(args[2]);
                uint32_t rs1_prime = r[0] - 8;
                uint32_t rs2_prime = r[1] - 8;
                encoded |= rs2_prime << 2;
                encoded |= ((imm >> 3) & 0x3) << 5;
                encoded |= rs1_prime << 7;
                encoded |= (imm & 0x7) << 10;
                encoded |= functHi << 13;
                break;
            }
            case IFormat::CA: {
                uint32_t rd_prime = r[0] - 8;
                uint32_t rs2_prime = r[1] - 8;
                encoded |= rs2_prime << 2;
                encoded |= functLo << 5;
                encoded |= rd_prime << 7;
                encoded |= functHi << 10;
                break;
            }
            case IFormat::CB: {
                uint32_t rs1_prime = r[0] - 8;
                if (mnemonic == "c.beqz" || mnemonic == "c.bnez") {
                    // imm was divided by 2 in resolve_symbols, multiply back to get byte offset
                    int32_t imm = std::get<int8>(args[1]) * 2;
                    encoded |= ((imm >> 5) & 0x1) << 2;
                    encoded |= ((imm >> 1) & 0x3) << 3;
                    encoded |= ((imm >> 6) & 0x3) << 5;
                    encoded |= rs1_prime << 7;
                    encoded |= ((imm >> 3) & 0x3) << 10;
                    encoded |= ((imm >> 8) & 0x1) << 12;
                    encoded |= functHi << 13;
                } else {
                    uint32_t imm = imm6_to_u32(args[1]);
                    encoded |= (imm & 0x1F) << 2;
                    encoded |= rs1_prime << 7;
                    encoded |= functLo << 10;
                    encoded |= ((imm >> 5) & 0x1) << 12;
                    encoded |= functHi << 13;
                }
                break;
            }
            case IFormat::CJ: {
                // imm was divided by 2 in resolve_symbols, multiply back to get byte offset
                int32_t imm = std::get<int11>(args[0]) * 2;
                encoded |= ((imm >> 5) & 0x1) << 2;
                encoded |= ((imm >> 1) & 0x7) << 3;
                encoded |= ((imm >> 7) & 0x1) << 6;
                encoded |= ((imm >> 6) & 0x1) << 7;
                encoded |= ((imm >> 10) & 0x1) << 8;
                encoded |= ((imm >> 8) & 0x3) << 9;
                encoded |= ((imm >> 4) & 0x1) << 11;
                encoded |= ((imm >> 11) & 0x1) << 12;
                encoded |= functHi << 13;
                break;
            }
        }

        if (size == 2) {
            if (endian != std::endian::native) {
                encoded = endianness::swap_endian(static_cast<uint16_t>(encoded));
            }
            return std::array<uint8_t, 2>{
                static_cast<uint8_t>(encoded),
                static_cast<uint8_t>(encoded >> 8)
            };
        }

        if (endian != std::endian::native) {
            encoded = endianness::swap_endian(encoded);
        }
        return std::array<uint8_t, 4>{
            static_cast<uint8_t>(encoded),
            static_cast<uint8_t>(encoded >> 8),
            static_cast<uint8_t>(encoded >> 16),
            static_cast<uint8_t>(encoded >> 24)
        };
    }
}
