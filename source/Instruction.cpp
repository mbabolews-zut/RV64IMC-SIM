#include <cassert>
#include <Instruction.hpp>
#include <rv64/instruction_sets/Rv64IMC.hpp>

#include "rv64/Cpu.hpp"

template<typename T>
concept HasMIN = requires { T::MIN; };

template<typename T>
static bool in_range(const std::string& value) {
    if constexpr (!HasMIN<T>)
        return T::MAX >= std::stoull(value);

    return T::MAX >= std::stoll(value)
           && T::MIN <= std::stoll(value);
}

Instruction::Instruction(rv64::Cpu &cpu, std::string_view mnemonic, const std::array<std::string_view, 3> &args) {
    auto proto = rv64::is::Rv64IMC::get_inst_proto(std::string(mnemonic));
    if (!proto) return;

    for (int i = 0; i < 3; ++i){
        std::string arg = std::string(args[i]);
        proto_id = -i - 1; // invalid by default

        if (proto.args[i] == InstArgType::None) break;
        if (proto.args[i] == InstArgType::IntReg) {
            auto reg_idx = rv64::IntReg::name_to_idx(arg);
            if (reg_idx == -1)
                return;
            this->args[i] = &cpu.get_int_reg(static_cast<size_t>(reg_idx));
            continue;
        }
        if (proto.args[i] == InstArgType::Imm12) {
            if (!in_range<int12>(arg)) return;
            this->args[i] = int12(std::stoll(arg));
        }
        else if (proto.args[i] == InstArgType::Imm20) {
            if (!in_range<int20>(arg)) return;
            this->args[i] = int20(std::stoll(arg));
        }
        else if (proto.args[i] == InstArgType::UImm5) {
            if (!in_range<uint5>(arg)) return;
            this->args[i] = uint5(std::stoull(arg));
        }
        else if (proto.args[i] == InstArgType::UImm6) {
            if (!in_range<uint6>(arg)) return;
            this->args[i] = uint6(std::stoull(arg));
        }
        else if (proto.args[i] == InstArgType::UImm12) {
            if (!in_range<uint12>(arg)) return;
            this->args[i] = uint12(std::stoull(arg));
        }
        else assert(false && "unreachable");
    }
    proto_id = proto.id;
}

