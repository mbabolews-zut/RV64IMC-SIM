#pragma once
#include <rv64/instruction_sets/IBase.hpp>
#include <rv64/instruction_sets/IExtM.hpp>
#include <Instruction.hpp>
#include <unordered_map>
#include <cassert>

namespace rv64::is {
    class Rv64IMC : public IBase, public IExtM {
    public:
        ~Rv64IMC() override = default;

        Rv64IMC() {
            if (instructions.empty()) {
                auto b = IBase::list_inst();
                auto m = IExtM::list_inst();
                instructions.reserve(b.size() + m.size());

                for (const auto &inst: b) {
                    instructions.emplace(std::string(inst.mnemonic), inst);
                }
                for (const auto &inst: m) {
                    instructions.emplace(std::string(inst.mnemonic), inst);
                }
                //TODO: C extension
            }
        }

        static const InstProto &get_inst_proto(const std::string &name) noexcept {

            auto it = instructions.find(to_lowercase(name));
            if (it == instructions.end())
                return invalid_inst_proto;
            return it->second;
        }

        static constexpr InstProto get_inst_proto(int id) noexcept {
            if (id >= IBase::BASE_ID && id < IBase::BASE_ID + IBase::list_inst().size()) {
                assert(IBase::list_inst().at(id - IBase::BASE_ID));
                return IBase::list_inst()[id - IBase::BASE_ID];
            }
            if (id >= IExtM::BASE_ID && id < IExtM::BASE_ID + IExtM::list_inst().size()) {
                return IExtM::list_inst()[id - IBase::BASE_ID];
            }
            return invalid_inst_proto;
        }

    protected:
        inline static std::unordered_map<std::string, InstProto> instructions;
    };
}
