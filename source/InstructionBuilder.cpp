#include "InstructionBuilder.hpp"
#include "rv64/instruction_sets/Rv64IMC.hpp"

template<typename T>
concept HasMIN = requires { T::MIN; };

template<typename T>
static bool in_range(int64_t value) {
    if constexpr (!HasMIN<T>) {
        return value >= 0 && static_cast<uint64_t>(value) <= static_cast<uint64_t>(T::MAX);
    } else {
        return value <= static_cast<int64_t>(T::MAX) && value >= static_cast<int64_t>(T::MIN);
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

bool InstructionBuilder::resolve_symbols(const std::unordered_map<std::string, uint64_t> &symbol_table) {
    for (size_t i = 0; i < m_arg_count; ++i) {
        if (auto *sym = std::get_if<UnresolvedSymbol>(&m_raw_args[i])) {
            auto it = symbol_table.find(sym->name);
            if (it == symbol_table.end()) {
                return false; // Symbol not found
            }
            m_raw_args[i] = static_cast<int64_t>(it->second) + sym->offset;
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
            return {}; // Not enough arguments
        }

        // Handle register arguments
        if (arg == InstArgType::IntReg || arg == InstArgType::IntRegP) {
            if (auto *str = std::get_if<std::string>(&m_raw_args[i])) {
                rv64::Reg reg(*str);
                if (!reg.is_valid()) {
                    return {};
                }
                auto ridx = reg.get_idx();
                if (arg == InstArgType::IntRegP && (ridx < 8 || ridx > 15)) {
                    return {}; // IntRegP must be in range x8-x15
                }
                validated_args[i] = reg;
            } else {
                return {}; // Expected register, got something else
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
