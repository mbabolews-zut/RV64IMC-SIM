#include <catch2/catch_all.hpp>
#include <rv64/VM.hpp>
#include <rv64/Cpu.hpp>
#include <rv64/Interpreter.hpp>

using namespace rv64;

TEST_CASE("RV64M multiplication instructions", "[rv64m][mul]") {
    VM vm{};
    Interpreter interp{vm};
    auto &cpu = vm.m_cpu;

    SECTION("mul - multiply") {
        cpu.reg(1) = 10;
        cpu.reg(2) = 20;
        interp.mul(cpu.reg(3), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(3) == 200);

        cpu.reg(4) = UINT64_MAX;
        cpu.reg(5) = 2;
        interp.mul(cpu.reg(6), cpu.reg(4), cpu.reg(5));
        REQUIRE(cpu.reg(6) == UINT64_MAX - 1);
    }

    SECTION("mulh - multiply high signed") {
        cpu.reg(1) = static_cast<uint64_t>(-1);
        cpu.reg(2) = static_cast<uint64_t>(-1);
        interp.mulh(cpu.reg(3), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(3) == 0);

        cpu.reg(4) = 0x8000000000000000;
        cpu.reg(5) = 2;
        interp.mulh(cpu.reg(6), cpu.reg(4), cpu.reg(5));
        REQUIRE(cpu.reg(6) == -1);
    }

    SECTION("mulhu - multiply high unsigned") {
        cpu.reg(1) = UINT64_MAX;
        cpu.reg(2) = UINT64_MAX;
        interp.mulhu(cpu.reg(3), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(3) == UINT64_MAX - 1);

        cpu.reg(4) = 0x8000000000000000;
        cpu.reg(5) = 2;
        interp.mulhu(cpu.reg(6), cpu.reg(4), cpu.reg(5));
        REQUIRE(cpu.reg(6) == 1);
    }

    SECTION("mulhsu - multiply high signed-unsigned") {
        cpu.reg(1) = static_cast<uint64_t>(-1);
        cpu.reg(2) = 2;
        interp.mulhsu(cpu.reg(3), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(3).sval() == -1);

        cpu.reg(4) = 0x7FFFFFFFFFFFFFFF;
        cpu.reg(5) = 2;
        interp.mulhsu(cpu.reg(6), cpu.reg(4), cpu.reg(5));
        REQUIRE(cpu.reg(6) == 0);
    }

    SECTION("mulw - multiply word") {
        cpu.reg(1) = 0x10000;
        cpu.reg(2) = 0x10000;
        interp.mulw(cpu.reg(3), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(3) == 0);

        cpu.reg(4) = 100;
        cpu.reg(5) = 200;
        interp.mulw(cpu.reg(6), cpu.reg(4), cpu.reg(5));
        REQUIRE(cpu.reg(6) == 20000);
    }

    SECTION("mul with zero") {
        cpu.reg(1) = 0;
        cpu.reg(2) = 100;
        interp.mul(cpu.reg(3), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(3) == 0);
    }

    SECTION("mul with negative numbers") {
        cpu.reg(1) = static_cast<uint64_t>(-5);
        cpu.reg(2) = 10;
        interp.mul(cpu.reg(3), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(3).sval() == -50);

        cpu.reg(4) = static_cast<uint64_t>(-5);
        cpu.reg(5) = static_cast<uint64_t>(-10);
        interp.mul(cpu.reg(6), cpu.reg(4), cpu.reg(5));
        REQUIRE(cpu.reg(6) == 50);
    }
}

TEST_CASE("RV64M division instructions", "[rv64m][div]") {
    VM vm{};
    Interpreter interp{vm};
    auto &cpu = vm.m_cpu;

    SECTION("div - divide signed") {
        cpu.reg(1) = 100;
        cpu.reg(2) = 10;
        interp.div(cpu.reg(3), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(3) == 10);

        cpu.reg(4) = static_cast<uint64_t>(-100);
        cpu.reg(5) = 10;
        interp.div(cpu.reg(6), cpu.reg(4), cpu.reg(5));
        REQUIRE(cpu.reg(6) == -10);

        cpu.reg(7) = 100;
        cpu.reg(8) = static_cast<uint64_t>(-10);
        interp.div(cpu.reg(9), cpu.reg(7), cpu.reg(8));
        REQUIRE(cpu.reg(9) == -10);
    }

    SECTION("div - division by zero") {
        cpu.reg(1) = 100;
        cpu.reg(2) = 0;
        interp.div(cpu.reg(3), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(3) == -1);
    }

    SECTION("div - overflow case") {
        cpu.reg(1) = 0x8000000000000000;
        cpu.reg(2) = static_cast<uint64_t>(-1);
        interp.div(cpu.reg(3), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(3) == 0x8000000000000000);
    }

    SECTION("divu - divide unsigned") {
        cpu.reg(1) = 100;
        cpu.reg(2) = 10;
        interp.divu(cpu.reg(3), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(3) == 10);

        cpu.reg(4) = UINT64_MAX;
        cpu.reg(5) = 2;
        interp.divu(cpu.reg(6), cpu.reg(4), cpu.reg(5));
        REQUIRE(cpu.reg(6) == 0x7FFFFFFFFFFFFFFF);
    }

    SECTION("divu - division by zero") {
        cpu.reg(1) = 100;
        cpu.reg(2) = 0;
        interp.divu(cpu.reg(3), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(3) == UINT64_MAX);
    }

    SECTION("divw - divide word signed") {
        cpu.reg(1) = 100;
        cpu.reg(2) = 10;
        interp.divw(cpu.reg(3), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(3) == 10);

        cpu.reg(4).val() = 0x80000000;
        cpu.reg(5) = static_cast<uint64_t>(-1);
        interp.divw(cpu.reg(6), cpu.reg(4), cpu.reg(5));
        REQUIRE(cpu.reg(6) == 0xFFFFFFFF80000000);
    }

    SECTION("divuw - divide word unsigned") {
        cpu.reg(1) = 100;
        cpu.reg(2) = 10;
        interp.divuw(cpu.reg(3), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(3) == 10);

        cpu.reg(4).val() = 0xFFFFFFFF;
        cpu.reg(5) = 2;
        interp.divuw(cpu.reg(6), cpu.reg(4), cpu.reg(5));
        REQUIRE(cpu.reg(6) == 0x7FFFFFFF);
    }
}

TEST_CASE("RV64M remainder instructions", "[rv64m][rem]") {
    VM vm{};
    Interpreter interp{vm};
    auto &cpu = vm.m_cpu;

    SECTION("rem - remainder signed") {
        cpu.reg(1) = 100;
        cpu.reg(2) = 30;
        interp.rem(cpu.reg(3), cpu.reg(1), cpu.reg(2));
        auto reg3_val = cpu.reg(3).sval();
        REQUIRE(reg3_val == 10);

        cpu.reg(4) = static_cast<uint64_t>(-100);
        cpu.reg(5) = 30;
        interp.rem(cpu.reg(6), cpu.reg(4), cpu.reg(5));
        REQUIRE(cpu.reg(6) == -10);
    }

    SECTION("rem - remainder by zero") {
        cpu.reg(1) = 100;
        cpu.reg(2) = 0;
        interp.rem(cpu.reg(3), cpu.reg(1), cpu.reg(2));
        auto reg3_val = cpu.reg(3).sval();
        REQUIRE(reg3_val == 100);
    }

    SECTION("remu - remainder unsigned") {
        cpu.reg(1) = 100;
        cpu.reg(2) = 30;
        interp.remu(cpu.reg(3), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(3) == 10);

        cpu.reg(4) = UINT64_MAX;
        cpu.reg(5) = 10;
        interp.remu(cpu.reg(6), cpu.reg(4), cpu.reg(5));
        REQUIRE(cpu.reg(6) == 5);
    }

    SECTION("remu - remainder by zero") {
        cpu.reg(1) = 100;
        cpu.reg(2) = 0;
        interp.remu(cpu.reg(3), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(3) == 100);
    }

    SECTION("remw - remainder word signed") {
        cpu.reg(1) = 100;
        cpu.reg(2) = 30;
        interp.remw(cpu.reg(3), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(3) == 10);
    }

    SECTION("remuw - remainder word unsigned") {
        cpu.reg(1) = 100;
        cpu.reg(2) = 30;
        interp.remuw(cpu.reg(3), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(3) == 10);
    }
}

TEST_CASE("RV64M edge cases", "[rv64m][edge_cases]") {
    VM vm{};
    Interpreter interp{vm};
    auto &cpu = vm.m_cpu;

    SECTION("Division overflow - INT64_MIN / -1") {
        cpu.reg(1) = static_cast<uint64_t>(INT64_MIN);
        cpu.reg(2) = static_cast<uint64_t>(-1);
        interp.div(cpu.reg(3), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(3) == static_cast<uint64_t>(INT64_MIN));
    }

    SECTION("Remainder overflow - INT64_MIN % -1") {
        cpu.reg(1) = static_cast<uint64_t>(INT64_MIN);
        cpu.reg(2) = static_cast<uint64_t>(-1);
        interp.rem(cpu.reg(3), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(3) == 0);
    }

    SECTION("32-bit division overflow - INT32_MIN / -1") {
        cpu.reg(1).val() = 0x80000000;
        cpu.reg(2) = static_cast<uint64_t>(-1);
        interp.divw(cpu.reg(3), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(3) == 0xFFFFFFFF80000000);
    }

    SECTION("Multiply large values") {
        cpu.reg(1) = 0x100000000;
        cpu.reg(2) = 0x100000000;
        interp.mul(cpu.reg(3), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(3) == 0);

        interp.mulhu(cpu.reg(4), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(4) == 1);
    }
}

