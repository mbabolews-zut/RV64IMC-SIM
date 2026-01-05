#include <catch2/catch_test_macros.hpp>
#include <parser/asm_parsing.hpp>
#include <rv64/VM.hpp>
#include <memory>

#include "ui.hpp"

using namespace rv64;

// Helper to parse, load, and run a program
static std::unique_ptr<VM> run_program(const std::string &source) {
    auto vm = std::make_unique<VM>();
    asm_parsing::ParsedInstVec instructions;
    int result = asm_parsing::parse_and_resolve(source, instructions, vm->m_cpu.get_pc());

    REQUIRE(result == 0);
    vm->load_program(instructions);
    vm->run_until_stop();
    return vm;
}

TEST_CASE("Integration - Simple arithmetic", "[integration]") {
    SECTION("add two registers") {
        auto vm = run_program(R"(
            addi x1, x0, 10
            addi x2, x0, 20
            add x3, x1, x2
        )");
        REQUIRE(vm->m_cpu.reg(1).val() == 10);
        REQUIRE(vm->m_cpu.reg(2) == 20);
        REQUIRE(vm->m_cpu.reg(3) == 30);
    }

    SECTION("subtract registers") {
        auto vm = run_program(R"(
            addi x1, x0, 100
            addi x2, x0, 30
            sub x3, x1, x2
        )");
        REQUIRE(vm->m_cpu.reg(3) == 70);
    }

    SECTION("multiply registers") {
        auto vm = run_program(R"(
            addi x1, x0, 7
            addi x2, x0, 6
            mul x3, x1, x2
        )");
        REQUIRE(vm->m_cpu.reg(3) == 42);
    }

    SECTION("division") {
        auto vm = run_program(R"(
            addi x1, x0, 100
            addi x2, x0, 7
            div x3, x1, x2
            rem x4, x1, x2
        )");
        REQUIRE(vm->m_cpu.reg(3) == 14); // 100 / 7 = 14
        REQUIRE(vm->m_cpu.reg(4) == 2); // 100 % 7 = 2
    }
}

TEST_CASE("Integration - Bitwise operations", "[integration]") {
    SECTION("and operation") {
        auto vm = run_program(R"(
            addi x1, x0, 0xFF
            addi x2, x0, 0x0F
            and x3, x1, x2
        )");
        REQUIRE(vm->m_cpu.reg(3) == 0x0F);
    }

    SECTION("or operation") {
        auto vm = run_program(R"(
            addi x1, x0, 0xF0
            addi x2, x0, 0x0F
            or x3, x1, x2
        )");
        REQUIRE(vm->m_cpu.reg(3) == 0xFF);
    }

    SECTION("xor operation") {
        auto vm = run_program(R"(
            addi x1, x0, 0xFF
            addi x2, x0, 0xAA
            xor x3, x1, x2
        )");
        REQUIRE(vm->m_cpu.reg(3) == 0x55);
    }

    SECTION("shift operations") {
        auto vm = run_program(R"(
            addi x1, x0, 1
            slli x2, x1, 4
            addi x3, x0, 256
            srli x4, x3, 4
        )");
        REQUIRE(vm->m_cpu.reg(2) == 16); // 1 << 4 = 16
        REQUIRE(vm->m_cpu.reg(4) == 16); // 256 >> 4 = 16
    }
}

TEST_CASE("Integration - Memory operations", "[integration]") {
    ui::set_error_msg_callback([](auto msg) {
            std::clog << msg;
        });
    SECTION("store and load word") {
        auto vm = run_program(R"(
            lui x1, 0x400
            addi x2, x0, 0x123
            sw x2, 0(x1)
            lw x3, 0(x1)
        )");
        REQUIRE(vm->m_cpu.reg(3).val() == 0x123);
    }

    SECTION("store and load doubleword") {
        auto vm = run_program(R"(
            lui x1, 0x400
            lui x2, 0x12345
            addi x2, x2, 0x678
            sd x2, 0(x1)
            ld x3, 0(x1)
        )");
        REQUIRE(vm->m_cpu.reg(3).val() == vm->m_cpu.reg(2).val());
    }

    SECTION("store and load byte") {
        auto vm = run_program(R"(
            lui x1, 0x400
            addi x2, x0, 0x7F
            sb x2, 0(x1)
            lb x3, 0(x1)
        )");
        REQUIRE(vm->m_cpu.reg(3).val() == 0x7F);
    }

    SECTION("load with offset syntax") {
        auto vm = run_program(R"(
            lui x1, 0x400
            addi x2, x0, 42
            sw x2, 8(x1)
            lw x3, 8(x1)
        )");
        REQUIRE(vm->m_cpu.reg(3) == 42);
    }
}

TEST_CASE("Integration - Branches", "[integration]") {
    SECTION("beq - branch taken") {
        auto vm = run_program(R"(
            addi x1, x0, 5
            addi x2, x0, 5
            beq x1, x2, skip
            addi x3, x0, 1
        skip:
            addi x4, x0, 2
        )");
        REQUIRE(vm->m_cpu.reg(3) == 0); // skipped
        REQUIRE(vm->m_cpu.reg(4) == 2); // executed
    }

    SECTION("beq - branch not taken") {
        auto vm = run_program(R"(
            addi x1, x0, 5
            addi x2, x0, 10
            beq x1, x2, skip
            addi x3, x0, 1
        skip:
            addi x4, x0, 2
        )");
        REQUIRE(vm->m_cpu.reg(3) == 1); // executed
        REQUIRE(vm->m_cpu.reg(4) == 2); // executed
    }

    SECTION("bne - branch taken") {
        auto vm = run_program(R"(
            addi x1, x0, 5
            addi x2, x0, 10
            bne x1, x2, skip
            addi x3, x0, 1
        skip:
            addi x4, x0, 2
        )");
        REQUIRE(vm->m_cpu.reg(3) == 0); // skipped
        REQUIRE(vm->m_cpu.reg(4) == 2); // executed
    }

    SECTION("blt - branch taken") {
        auto vm = run_program(R"(
            addi x1, x0, 5
            addi x2, x0, 10
            blt x1, x2, skip
            addi x3, x0, 1
        skip:
            addi x4, x0, 2
        )");
        REQUIRE(vm->m_cpu.reg(3) == 0); // skipped
        REQUIRE(vm->m_cpu.reg(4) == 2); // executed
    }

    SECTION("bge - branch taken") {
        auto vm = run_program(R"(
            addi x1, x0, 10
            addi x2, x0, 5
            bge x1, x2, skip
            addi x3, x0, 1
        skip:
            addi x4, x0, 2
        )");
        REQUIRE(vm->m_cpu.reg(3) == 0); // skipped
        REQUIRE(vm->m_cpu.reg(4) == 2); // executed
    }
}

TEST_CASE("Integration - Loops", "[integration]") {
    SECTION("count down loop") {
        auto vm = run_program(R"(
            addi x1, x0, 10
            addi x2, x0, 0
        loop:
            addi x2, x2, 1
            addi x1, x1, -1
            bne x1, x0, loop
        )");
        REQUIRE(vm->m_cpu.reg(1) == 0); // counter reached 0
        REQUIRE(vm->m_cpu.reg(2) == 10); // iterated 10 times
    }

    SECTION("sum 1 to N") {
        auto vm = run_program(R"(
            addi x1, x0, 10
            addi x2, x0, 0
        loop:
            add x2, x2, x1
            addi x1, x1, -1
            bne x1, x0, loop
        )");
        REQUIRE(vm->m_cpu.reg(2) == 55); // 1+2+...+10 = 55
    }

    SECTION("factorial") {
        auto vm = run_program(R"(
            addi x1, x0, 5
            addi x2, x0, 1
        loop:
            mul x2, x2, x1
            addi x1, x1, -1
            bne x1, x0, loop
        )");
        REQUIRE(vm->m_cpu.reg(2) == 120); // 5! = 120
    }
}

TEST_CASE("Integration - Function calls", "[integration]") {
    SECTION("jal and jalr") {
        auto vm = run_program(R"(
            addi x10, x0, 5
            addi x9, x0, 7
            jal x1, func
            addi x11, x11, 5
            bge x11, x9, end
        func:
            addi x10, x10, 10
            jalr x0, x1, 0
        end:
        )");
        REQUIRE(vm->m_cpu.reg(10).val() == 25); // 5 + 10 + 10
        REQUIRE(vm->m_cpu.reg(11) == 10);
    }

}

TEST_CASE("Integration - Register aliases", "[integration]") {
    SECTION("ABI register names") {
        auto vm = run_program(R"(
            addi a0, zero, 10
            addi a1, zero, 20
            add a2, a0, a1
            addi t0, zero, 5
            addi t1, zero, 3
            mul t2, t0, t1
        )");
        REQUIRE(vm->m_cpu.reg(10) == 10); // a0
        REQUIRE(vm->m_cpu.reg(11) == 20); // a1
        REQUIRE(vm->m_cpu.reg(12) == 30); // a2
        REQUIRE(vm->m_cpu.reg(5) == 5); // t0
        REQUIRE(vm->m_cpu.reg(6) == 3); // t1
        REQUIRE(vm->m_cpu.reg(7) == 15); // t2
    }

    SECTION("stack pointer operations") {
        auto vm = run_program(R"(
            addi t0, sp, 0
            addi sp, sp, -32
            addi t1, sp, 0
            addi sp, sp, 32
            addi t2, sp, 0
        )");
        REQUIRE(vm->m_cpu.reg(5).val() == vm->m_cpu.reg(7).val()); // t0 == t2 (sp restored)
        REQUIRE(vm->m_cpu.reg(6).val() == vm->m_cpu.reg(5).val() - 32); // t1 = original sp - 32
    }
}

TEST_CASE("Integration - 64-bit operations", "[integration]") {
    SECTION("lui for large values") {
        auto vm = run_program(R"(
            lui x1, 0xFFFFF
            addi x1, x1, 0x7FF
        )");
        REQUIRE(vm->m_cpu.reg(1).as_u32() == 0xFFFFF7FF);
    }

    SECTION("64-bit arithmetic") {
        auto vm = run_program(R"(
            lui x1, 0x10000
            slli x1, x1, 20
            addi x2, x0, 1
            add x3, x1, x2
        )");
        uint64_t expected = (0x10000ULL << 12) << 20;
        REQUIRE(vm->m_cpu.reg(1) == expected);
        REQUIRE(vm->m_cpu.reg(3) == expected + 1);
    }

    SECTION("word operations sign extend") {
        // Test that addw sign-extends 32-bit result to 64 bits
        auto vm = run_program(R"(
            lui x1, 0x80000
            srli x1, x1, 12
            addw x2, x1, x0
        )");
        REQUIRE(vm->m_cpu.reg(1).val() == 0x000ffffffff80000);
        REQUIRE(vm->m_cpu.reg(2).val() == 0xfffffffffff80000);
    }
}

TEST_CASE("Integration - Edge cases", "[integration]") {
    SECTION("x0 always zero") {
        auto vm = run_program(R"(
            addi x0, x0, 100
            add x0, x0, x0
            addi x1, x0, 50
        )");
        REQUIRE(vm->m_cpu.reg(0) == 0);
        REQUIRE(vm->m_cpu.reg(1) == 50);
    }

    SECTION("negative numbers") {
        auto vm = run_program(R"(
            addi x1, x0, -10
            addi x2, x0, -20
            add x3, x1, x2
        )");
        REQUIRE(vm->m_cpu.reg(1).sval() == -10);
        REQUIRE(vm->m_cpu.reg(2).sval() == -20);
        REQUIRE(vm->m_cpu.reg(3).sval() == -30);
    }

    SECTION("signed comparison") {
        auto vm = run_program(R"(
            addi x1, x0, -5
            addi x2, x0, 5
            slt x3, x1, x2
            sltu x4, x1, x2
        )");
        REQUIRE(vm->m_cpu.reg(3) == 1); // -5 < 5 (signed)
        REQUIRE(vm->m_cpu.reg(4) == 0); // -5 as unsigned is very large
    }
}

TEST_CASE("Integration - Unsigned branches", "[integration]") {
    SECTION("bltu - branch taken") {
        auto vm = run_program(R"(
            addi x1, x0, 5
            addi x2, x0, 10
            bltu x1, x2, skip
            addi x3, x0, 1
        skip:
            addi x4, x0, 2
        )");
        REQUIRE(vm->m_cpu.reg(3) == 0); // skipped
        REQUIRE(vm->m_cpu.reg(4) == 2); // executed
    }

    SECTION("bltu - branch not taken (negative as unsigned is large)") {
        auto vm = run_program(R"(
            addi x1, x0, -5
            addi x2, x0, 10
            bltu x1, x2, skip
            addi x3, x0, 1
        skip:
            addi x4, x0, 2
        )");
        REQUIRE(vm->m_cpu.reg(3) == 1); // executed (-5 as unsigned > 10)
        REQUIRE(vm->m_cpu.reg(4) == 2); // executed
    }

    SECTION("bgeu - branch taken") {
        auto vm = run_program(R"(
            addi x1, x0, 10
            addi x2, x0, 5
            bgeu x1, x2, skip
            addi x3, x0, 1
        skip:
            addi x4, x0, 2
        )");
        REQUIRE(vm->m_cpu.reg(3) == 0); // skipped
        REQUIRE(vm->m_cpu.reg(4) == 2); // executed
    }

    SECTION("bgeu - branch taken (negative as unsigned is large)") {
        auto vm = run_program(R"(
            addi x1, x0, -5
            addi x2, x0, 10
            bgeu x1, x2, skip
            addi x3, x0, 1
        skip:
            addi x4, x0, 2
        )");
        REQUIRE(vm->m_cpu.reg(3) == 0); // skipped (-5 as unsigned >= 10)
        REQUIRE(vm->m_cpu.reg(4) == 2); // executed
    }

    SECTION("bgeu - equal values") {
        auto vm = run_program(R"(
            addi x1, x0, 10
            addi x2, x0, 10
            bgeu x1, x2, skip
            addi x3, x0, 1
        skip:
            addi x4, x0, 2
        )");
        REQUIRE(vm->m_cpu.reg(3) == 0); // skipped (10 >= 10)
        REQUIRE(vm->m_cpu.reg(4) == 2); // executed
    }
}

TEST_CASE("Integration - System calls", "[integration]") {
    SECTION("ecall with a0=10 exits program") {
        auto vm = run_program(R"(
            addi x1, x0, 42
            addi a0, x0, 10
            ecall
            addi x3, x0, 99
        )");
        REQUIRE(vm->m_cpu.reg(1) == 42); // executed before ecall
        REQUIRE(vm->m_cpu.reg(3) == 0);  // not executed (after ecall)
        REQUIRE(vm->get_state() == VMState::Finished);
    }

    SECTION("program execution stops on ecall") {
        auto vm = run_program(R"(
            addi x1, x0, 1
            addi x2, x0, 2
            addi a0, x0, 10
            ecall
            addi x3, x0, 3
            addi x4, x0, 4
        )");
        REQUIRE(vm->m_cpu.reg(1) == 1);
        REQUIRE(vm->m_cpu.reg(2) == 2);
        REQUIRE(vm->m_cpu.reg(3) == 0); // not executed
        REQUIRE(vm->m_cpu.reg(4) == 0); // not executed
    }
}
