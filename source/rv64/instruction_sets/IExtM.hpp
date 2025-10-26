#pragma once

namespace rv64 {
    class IntReg;
}

namespace rv64::is {
    class IExtM {
    public:
        static constexpr int BASE_ID = 200;

        /// @brief multiply
        /// <br> rd = rs1 * rs2
        virtual void mul(IntReg &rd, const IntReg &rs1, const IntReg &rs2) = 0;

        /// @brief multiply, store 128-bit upper half in rd (signed registers)
        /// <br> i128 t = rs1 * rs2;
        /// <br> rd = t[127:64]
        virtual void mulh(IntReg &rd, const IntReg &rs1, const IntReg &rs2) = 0;

        /// @brief multiply, store 128-bit upper half in rd (unsigned registers)
        /// <br> u128 t = rs1 * rs2;
        /// <br> rd = t[127:64]
        virtual void mulhu(IntReg &rd, const IntReg &rs1, const IntReg &rs2) = 0;

        /// @brief multiply, store 128-bit upper half in rd (rs1 - signed, rs2 - unsigned)
        /// <br> i128 t = signed(rs1) * unsigned(rs2);
        /// <br> rd = t[127:64]
        virtual void mulhsu(IntReg &rd, const IntReg &rs1, const IntReg &rs2) = 0;

        /// @brief multiply 32-bit registers and sign-extend the result to 64 bits.
        /// <br> rd = sign_extend<i64>(rs1[31:0] * rs2[31:0])
        virtual void mulw(IntReg &rd, const IntReg &rs1, const IntReg &rs2) = 0;

        /// @brief divide (signed registers)
        /// <br> rd = rs1 / rs2
        virtual void div(IntReg &rd, const IntReg &rs1, const IntReg &rs2) = 0;

        /// @brief divide (unsigned registers)
        /// <br> rd = rs1 / rs2
        virtual void divu(IntReg &rd, const IntReg &rs1, const IntReg &rs2) = 0;

        /// @brief remainder (signed registers)
        /// <br> rd = rs1 % rs2
        virtual void rem(IntReg &rd, const IntReg &rs1, const IntReg &rs2) = 0;

        /// @brief remainder (unsigned registers)
        /// <br> rd = rs1 % rs2
        virtual void remu(IntReg &rd, const IntReg &rs1, const IntReg &rs2) = 0;

        /// @brief divide 32-bit registers and sign-extend the result to 64 bits.
        /// <br> rd = sign_extend<i64>(rs1[31:0] / rs2[31:0])
        virtual void divw(IntReg &rd, const IntReg &rs1, const IntReg &rs2) = 0;

        /// @brief divide 32-bit registers and zero-extend the result to 64 bits. (unsigned registers)
        /// <br> rd = sign_extend<i64>(rs1[31:0] / rs2[31:0])
        virtual void divuw(IntReg &rd, const IntReg &rs1, const IntReg &rs2) = 0;

        /// @brief remainder 32-bit registers and sign-extend the result to 64 bits. (signed registers)
        /// <br> rd = sign_extend<i64>(rs1[31:0] % rs2[31:0])
        virtual void remw(IntReg &rd, const IntReg &rs1, const IntReg &rs2) = 0;

        /// @brief remainder 32-bit registers and zero-extend the result to 64 bits. (unsigned registers)
        /// <br> rd = sign_extend<i64>(rs1[31:0] % rs2[31:0])
        virtual void remuw(IntReg &rd, const IntReg &rs1, const IntReg &rs2) = 0;


        static constexpr std::array<InstProto, 13> list_inst() {
            const auto ireg = InstArgType::IntReg;
            return {{
                {"mul"sv, {ireg, ireg, ireg}, (int)InstId::mul},
                {"mulh"sv, {ireg, ireg, ireg}, (int)InstId::mulh},
                {"mulhu"sv, {ireg, ireg, ireg}, (int)InstId::mulhu},
                {"mulhsu"sv, {ireg, ireg, ireg}, (int)InstId::mulhsu},
                {"mulw"sv, {ireg, ireg, ireg}, (int)InstId::mulw},
                {"div"sv, {ireg, ireg, ireg}, (int)InstId::div},
                {"divu"sv, {ireg, ireg, ireg}, (int)InstId::divu},
                {"rem"sv, {ireg, ireg, ireg}, (int)InstId::rem},
                {"remu"sv, {ireg, ireg, ireg}, (int)InstId::remu},
                {"divw"sv, {ireg, ireg, ireg}, (int)InstId::divw},
                {"divuw"sv, {ireg, ireg, ireg}, (int)InstId::divuw},
                {"remw"sv, {ireg, ireg, ireg}, (int)InstId::remw},
                {"remuw"sv, {ireg, ireg, ireg}, (int)InstId::remuw},
                }};
        }

        static_assert(BASE_ID > 0);
        enum class InstId {
            mul = BASE_ID, mulh, mulhu, mulhsu, mulw, div,
            divu, rem, remu, divw, divuw, remw, remuw
        };

    };
}
