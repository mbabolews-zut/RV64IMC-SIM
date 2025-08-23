#pragma once
#include <cstdint>
#include <string>
#include <array>

namespace rv64 {
    class IntReg {
    public:
        uint64_t val() const;
        uint64_t &val();
        int64_t sval() const;
        int64_t &sval();
        int32_t as_i32() const;
        uint32_t as_u32() const;
        std::string get_name() const;
        std::string get_sym_name() const;
        IntReg &operator=(uint64_t val);
        IntReg &operator=(int64_t val);
        IntReg &operator=(int32_t val);

        explicit IntReg(size_t idx);

    private:
        uint64_t m_value = 0;
        const size_t m_idx;
    };

    class Cpu {
    public:
        static constexpr size_t INT_REG_CNT = 32;
        static constexpr size_t FP_REG_CNT = 32;

        void set_pc(uint64_t pc);

        IntReg &get_int_reg(size_t i) noexcept;

        const IntReg &get_int_reg(size_t i) const noexcept;

        uint64_t get_pc() const;

        Cpu();

    private:
        template<class T, std::size_t... Is>
        static constexpr std::array<T, sizeof...(Is)>
        reg_array_construct(std::index_sequence<Is...>) {
            return {IntReg(Is)...};
        }

    private:
        std::array<IntReg, 32> m_int_regs;
        double m_fp_regs[FP_REG_CNT]{};
        uint64_t m_pc = 0;
    };

}
