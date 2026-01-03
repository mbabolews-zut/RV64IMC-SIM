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
        return value <= T::MAX && value >= T::MIN;
    }
}

template<typename T>
static std::optional<BuildError> try_parse_intN(int64_t value, InstArg &out, std::string_view mnemonic) {
    if (in_range<T>(value)) {
        out = T(value);
        return std::nullopt;
    }
    return BuildError{
        .kind = BuildErrorKind::ImmediateOutOfRange,
        .message = std::format("Immediate {} out of range [{}, {}] for '{}'",
                               value, T::MIN, T::MAX, mnemonic)
    };
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

std::optional<BuildError> InstructionBuilder::resolve_symbols(const std::unordered_map<std::string, uint64_t> &symbol_table, uint64_t current_pc) {
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
                return BuildError{
                    .kind = BuildErrorKind::UnresolvedSymbol,
                    .message = std::format("Unresolved symbol '{}'", sym->name)
                };
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
    return std::nullopt;
}

std::variant<Instruction, BuildError> InstructionBuilder::build() const {
    auto proto = rv64::is::Rv64IMC::get_inst_proto(m_mnemonic);
    if (!proto) {
        return BuildError{
            .kind = BuildErrorKind::UnknownMnemonic,
            .message = std::format("Unknown instruction '{}'", m_mnemonic)
        };
    }

    std::array<InstArg, 3> validated_args;

    for (size_t i = 0; i < 3; ++i) {
        auto arg = proto.args[i];
        if (arg == InstArgType::None) {
            break;
        }

        if (i >= m_arg_count) {
            return BuildError{
                .kind = BuildErrorKind::MissingArgument,
                .message = std::format("Missing argument {} for '{}'", i + 1, m_mnemonic)
            };
        }

        if (arg == InstArgType::IntReg || arg == InstArgType::IntRegP) {
            if (auto *str = std::get_if<std::string>(&m_raw_args[i])) {
                rv64::Reg reg(*str);
                if (!reg.is_valid()) {
                    return BuildError{
                        .kind = BuildErrorKind::InvalidRegister,
                        .message = std::format("Invalid register '{}'", *str)
                    };
                }

                if (arg == InstArgType::IntRegP && !reg.in_compressed_range()) {
                    return BuildError{
                        .kind = BuildErrorKind::RegisterNotInCompressedRange,
                        .message = std::format("Register '{}' not in compressed range (x8-x15) for '{}'",
                                               *str, m_mnemonic)
                    };
                }
                validated_args[i] = reg;
            } else {
                return BuildError{
                    .kind = BuildErrorKind::InvalidRegister,
                    .message = std::format("Expected register for argument {} of '{}'", i + 1, m_mnemonic)
                };
            }
            continue;
        }


        // Handle immediate arguments
        int64_t imm_value = 0;
        if (auto *val = std::get_if<int64_t>(&m_raw_args[i])) {
            imm_value = *val;
        } else if (auto *str = std::get_if<std::string>(&m_raw_args[i])) {
            if (!try_parse_immediate(*str, imm_value)) {
                return BuildError{
                    .kind = BuildErrorKind::ImmediateOutOfRange,
                    .message = std::format("Cannot parse '{}' as immediate", *str)
                };
            }
        } else if (std::get_if<UnresolvedSymbol>(&m_raw_args[i])) {
            return BuildError{
                .kind = BuildErrorKind::UnresolvedSymbol,
                .message = std::format("Unresolved symbol in argument {} of '{}'", i + 1, m_mnemonic)
            };
        }

        // Validate range and create typed immediate
        std::optional<BuildError> range_err;
        switch (arg) {
            case InstArgType::Imm12:
                range_err = try_parse_intN<int12>(imm_value, validated_args[i], m_mnemonic);
                break;
            case InstArgType::Imm20:
                range_err = try_parse_intN<int20>(imm_value, validated_args[i], m_mnemonic);
                break;
            case InstArgType::UImm5:
                range_err = try_parse_intN<uint5>(imm_value, validated_args[i], m_mnemonic);
                break;
            case InstArgType::UImm6:
                range_err = try_parse_intN<uint6>(imm_value, validated_args[i], m_mnemonic);
                break;
            case InstArgType::UImm12:
                range_err = try_parse_intN<uint12>(imm_value, validated_args[i], m_mnemonic);
                break;
            case InstArgType::UImm20:
                range_err = try_parse_intN<uint20>(imm_value, validated_args[i], m_mnemonic);
                break;
            case InstArgType::Imm5:
                range_err = try_parse_intN<int5>(imm_value, validated_args[i], m_mnemonic);
                break;
            case InstArgType::Imm6:
                range_err = try_parse_intN<int6>(imm_value, validated_args[i], m_mnemonic);
                break;
            case InstArgType::Imm8:
                range_err = try_parse_intN<int8>(imm_value, validated_args[i], m_mnemonic);
                break;
            case InstArgType::Imm11:
                range_err = try_parse_intN<int11>(imm_value, validated_args[i], m_mnemonic);
                break;
            case InstArgType::UImm8:
                range_err = try_parse_intN<uint8>(imm_value, validated_args[i], m_mnemonic);
                break;
            default:
                assert(false && "Unhandled InstArgType in InstructionBuilder::build()");
        }
        if (range_err) return *range_err;
    }

    return Instruction{proto.id, validated_args};
}

void InstructionBuilder::reset() {
    m_mnemonic.clear();
    m_raw_args = {};
    m_arg_count = 0;
}
