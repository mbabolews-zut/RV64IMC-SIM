#pragma once
#include <cstdint>
#include <string>

namespace rv64 {
    class IntReg {
    public:
        [[nodiscard]] uint64_t val() const;
        [[nodiscard]] uint64_t &val();
        [[nodiscard]] int64_t sval() const;
        [[nodiscard]] int64_t &sval();
        [[nodiscard]] int32_t as_i32() const;
        [[nodiscard]] uint32_t as_u32() const;
        [[nodiscard]] std::string get_name() const;
        [[nodiscard]] std::string get_sym_name() const;
        IntReg &operator=(uint64_t val);
        IntReg &operator=(int64_t val);
        IntReg &operator=(int32_t val);

        explicit IntReg(size_t idx, bool owned_by_cpu = false);

        /// @brief converts register (symbolic or not) name to index. Returns -1 if the name is invalid.
        [[nodiscard]] static int name_to_idx(const std::string &name);

    private:
        uint64_t m_value = 0;
        const bool m_owned_by_cpu = false;
        const size_t m_idx;
    };
}