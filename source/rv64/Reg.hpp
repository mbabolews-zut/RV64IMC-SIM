#pragma once
#include <cstdint>
#include <string>

namespace rv64 {
    class Reg {
    public:
        explicit Reg(int idx);

        /// @brief constructs register from its name (symbolic or not).
        /// * If the name is invalid, the register will be invalid (i.e., get_idx() == -1).
        /// @attention Check validity with is_valid() or operator bool().
        explicit Reg(std::string_view name);

        [[nodiscard]] std::string get_name() const;
        [[nodiscard]] std::string get_sym_name() const;

        /// @brief converts register (symbolic or not) name to index. Returns -1 if the name is invalid.
        [[nodiscard]] static int name_to_idx(const std::string &name);

        [[nodiscard]] int get_idx() const;

        [[nodiscard]] bool is_valid() const;

        explicit operator bool() const;

    private:
        int m_idx;
    };
}
