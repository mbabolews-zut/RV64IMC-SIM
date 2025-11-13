#pragma once
#include <rv64/instruction_sets/IBaseI.hpp>
#include <rv64/instruction_sets/IExtensionM.hpp>
#include <rv64/instruction_sets/IExtensionC.hpp>
#include <Instruction.hpp>
#include <unordered_map>
#include <cassert>


namespace rv64::is {
    class Rv64IMC : public IBaseI, public IExtensionM, public IExtensionC {
    public:
        ~Rv64IMC() override = default;

        Rv64IMC() {
            if (instructions.empty()) {
                auto b = IBaseI::list_inst();
                auto m = IExtensionM::list_inst();
                auto c = IExtensionC::list_inst();
                instructions.reserve(b.size() + m.size() + c.size());

                for (const auto &inst: b) {
                    instructions.emplace(std::string(inst.mnemonic), inst);
                }
                for (const auto &inst: m) {
                    instructions.emplace(std::string(inst.mnemonic), inst);
                }
                for (const auto &inst: c) {
                    instructions.emplace(std::string(inst.mnemonic), inst);
                }
            }
        }

        static const InstProto &get_inst_proto(const std::string &name) noexcept {

            auto it = instructions.find(to_lowercase(name));
            if (it == instructions.end())
                return invalid_inst_proto;
            return it->second;
        }

        static constexpr InstProto get_inst_proto(int id) noexcept {
            // Integer Base
            if (id >= IBaseI::IS_ID && id < IBaseI::IS_ID + IBaseI::list_inst().size()) {
                return IBaseI::list_inst()[id - IBaseI::IS_ID];
            }
            // M-extension
            if (id >= IExtensionM::IS_ID && id < IExtensionM::IS_ID + IExtensionM::list_inst().size()) {
                return IExtensionM::list_inst()[id - IExtensionM::IS_ID];
            }
            // C-extension
            if (id >= IExtensionC::IS_ID && id < IExtensionC::IS_ID + IExtensionC::list_inst().size()) {
                return IExtensionC::list_inst()[id - IExtensionC::IS_ID];
            }
            return invalid_inst_proto;
        }

    protected:
        inline static std::unordered_map<std::string, InstProto> instructions;
    };
}
