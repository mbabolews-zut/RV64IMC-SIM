#include "InstructionBuilder.hpp"

#include <format>

#include "rv64/instruction_sets/Rv64IMC.hpp"

template<typename T>
concept HasMIN = requires { T::MIN; };

template<typename T>
static bool in_range(int64_t value) {
    if constexpr (!HasMIN<T>) {
        return value >= 0 && value <= T::MAX;
    } else {
        std::clog << std::format("value={}, T::MIN={}, T::MAX={}, value <= T::MAX: {}, value >= T::MIN={}\n",
            value, T::MIN, T::MAX, value <= T::MAX, value >= T::MIN);
        return value <= T::MAX && value >= T::MIN;
    }
}

template<typename T>
static bool try_parse_intN(int64_t value, InstArg &out) {
    if (in_range<T>(value)) {
        out = T(value);
        return true;
    }
    return false;
}

static bool try_parse_immediate(std::string_view str, int64_t &out) {
    try {
        if (str.starts_with("0b") || str.starts_with("0B")) {
            out = std::stoll(std::string(str.substr(2)), nullptr, 2);
        } else {
            out = std::stoll(std::string(str), nullptr, 0);
        }
        return true;
    } catch (...) {
        return false;
    }
}


InstructionBuilder::InstructionBuilder(std::string_view mnemonic)
    : m_mnemonic(to_lowercase(std::string(mnemonic))) {
}

InstructionBuilder &InstructionBuilder::set_mnemonic(std::string_view mnemonic) {
    m_mnemonic = to_lowercase(std::string(mnemonic));
    return *this;
}

InstructionBuilder &InstructionBuilder::add_arg(std::string_view arg) {
    if (m_arg_count >= 3) return *this;

    std::string arg_str(arg);

    // Try to parse as immediate first
    int64_t imm_value;
    if (try_parse_immediate(arg_str, imm_value)) {
        m_raw_args[m_arg_count++] = imm_value;
        return *this;
    }

    // Otherwise store as string (register name or symbol)
    m_raw_args[m_arg_count++] = arg_str;
    return *this;
}

InstructionBuilder &InstructionBuilder::add_imm(int64_t value) {
    if (m_arg_count >= 3) return *this;
    m_raw_args[m_arg_count++] = value;
    return *this;
}

InstructionBuilder &InstructionBuilder::add_symbol(std::string_view symbol, int64_t offset) {
    if (m_arg_count >= 3) return *this;
    m_raw_args[m_arg_count++] = UnresolvedSymbol{std::string(symbol), offset};
    return *this;
}

bool InstructionBuilder::resolve_symbols(const std::unordered_map<std::string, uint64_t> &symbol_table, uint64_t current_pc) {
    // PC-relative instruction metadata (jalr, c.jr, c.jalr are NOT here - they use absolute addressing)
    struct InstrMeta {
        int arg_index;      // which argument is the PC-relative offset
        int offset_divisor; // divide byte offset by this (2 for half-word encoding)
    };

    static const std::unordered_map<std::string_view, InstrMeta> pc_relative_instrs{
        {"beq",      {2, 2}},
        {"bne",      {2, 2}},
        {"blt",      {2, 2}},
        {"bltu",     {2, 2}},
        {"bge",      {2, 2}},
        {"bgeu",     {2, 2}},
        {"jal",      {1, 2}},
        {"c.j",      {0, 2}},
        {"c.beqz",   {1, 2}},
        {"c.bnez",   {1, 2}},
    };

    auto meta_it = pc_relative_instrs.find(m_mnemonic);

    // Determine instruction size (compressed vs standard)
    bool is_compressed = m_mnemonic.size() >= 2 && m_mnemonic[0] == 'c' && m_mnemonic[1] == '.';
    uint64_t instr_size = is_compressed ? 2 : 4;

    for (size_t i = 0; i < m_arg_count; ++i) {
        if (auto *sym = std::get_if<UnresolvedSymbol>(&m_raw_args[i])) {
            auto it = symbol_table.find(sym->name);
            if (it == symbol_table.end()) {
                return false; // Symbol not found
            }

            uint64_t target_address = it->second + sym->offset;

            // If this instruction is in the PC-relative table, and we're at the offset argument
            if (meta_it != pc_relative_instrs.end() && static_cast<int>(i) == meta_it->second.arg_index) {
                // PC-relative offset: calculate from next instruction's PC
                uint64_t next_pc = current_pc + instr_size;
                int64_t byte_offset = static_cast<int64_t>(target_address) - static_cast<int64_t>(next_pc);
                m_raw_args[i] = byte_offset / meta_it->second.offset_divisor;
            } else {
                // Absolute address (for non-PC-relative instructions like jalr, or data labels)
                m_raw_args[i] = static_cast<int64_t>(target_address);
            }
        }
    }
    return true;
}

Instruction InstructionBuilder::build() const {
    auto proto = rv64::is::Rv64IMC::get_inst_proto(m_mnemonic);
    if (!proto) {
        return {};
    }

    std::array<InstArg, 3> validated_args;

    for (size_t i = 0; i < 3; ++i) {
        auto arg = proto.args[i];
        if (arg == InstArgType::None) {
            break;
        }

        if (i >= m_arg_count) {
            return {};
        }

        if (arg == InstArgType::IntReg || arg == InstArgType::IntRegP) {
            if (auto *str = std::get_if<std::string>(&m_raw_args[i])) {
                rv64::Reg reg(*str);
                if (!reg.is_valid()) {
                    return {};
                }

                if (arg == InstArgType::IntRegP && !reg.in_compressed_range()) {
                    return {};
                }
                validated_args[i] = reg;
            } else {
                return {};
            }
            continue;
        }


        // Handle immediate arguments
        int64_t imm_value = 0;
        if (auto *val = std::get_if<int64_t>(&m_raw_args[i])) {
            imm_value = *val;
        } else if (auto *str = std::get_if<std::string>(&m_raw_args[i])) {
            if (!try_parse_immediate(*str, imm_value)) {
                return {};
            }
        } else {
            return {}; // Unresolved symbol or invalid type
        }

        // Branch instructions encode PC-relative offsets differently
        // The immediate is already encoded correctly by the assembler/parser
        // so no need to divide by 2 here - that happens in the parser

        // Validate range and create typed immediate
        switch (arg) {
            case InstArgType::Imm12:
                if (!try_parse_intN<int12>(imm_value, validated_args[i])) return {};
                break;
            case InstArgType::Imm20:
                if (!try_parse_intN<int20>(imm_value, validated_args[i])) return {};
                break;
            case InstArgType::UImm5:
                if (!try_parse_intN<uint5>(imm_value, validated_args[i])) return {};
                break;
            case InstArgType::UImm6:
                if (!try_parse_intN<uint6>(imm_value, validated_args[i])) return {};
                break;
            case InstArgType::UImm12:
                if (!try_parse_intN<uint12>(imm_value, validated_args[i])) return {};
                break;
            case InstArgType::UImm20:
                if (!try_parse_intN<uint20>(imm_value, validated_args[i])) return {};
                break;
            case InstArgType::Imm5:
                if (!try_parse_intN<int5>(imm_value, validated_args[i])) return {};
                break;
            case InstArgType::Imm6:
                if (!try_parse_intN<int6>(imm_value, validated_args[i])) return {};
                break;
            case InstArgType::Imm8:
                if (!try_parse_intN<int8>(imm_value, validated_args[i])) return {};
                break;
            case InstArgType::Imm11:
                if (!try_parse_intN<int11>(imm_value, validated_args[i])) return {};
                break;
            case InstArgType::UImm8:
                if (!try_parse_intN<uint8>(imm_value, validated_args[i])) return {};
                break;
            default:
                assert(false && "Unhandled InstArgType in InstructionBuilder::build()");
        }
    }

    return {proto.id, validated_args};
}

void InstructionBuilder::reset() {
    m_mnemonic.clear();
    m_raw_args = {};
    m_arg_count = 0;
}
