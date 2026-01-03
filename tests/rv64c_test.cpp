#include <catch2/catch_all.hpp>
#include <rv64/VM.hpp>
#include <rv64/Cpu.hpp>
#include <rv64/Interpreter.hpp>

using namespace rv64;

TEST_CASE("RV64C load/store stack-pointer relative", "[rv64c][memory][stack]") {
    VM vm{};
    Interpreter interp{vm};
    auto &cpu = vm.m_cpu;
    Memory &mem = vm.m_memory;
    const auto &layout = vm.get_memory_layout();
    MemErr err = MemErr::None;

    // Set stack pointer
    cpu.reg(2) = layout.stack_base;

    SECTION("c.lwsp - load word stack-pointer relative") {
        err = mem.store<uint32_t>(layout.stack_base + 16, 0x12345678);
        REQUIRE(err == MemErr::None);
        REQUIRE_NOTHROW(interp.c_lwsp(cpu.reg(10), 4)); // offset = 4*4 = 16
        REQUIRE(cpu.reg(10) == 0x12345678);
    }

    SECTION("c.lwsp - sign extension") {
        err = mem.store<uint32_t>(layout.stack_base + 8, 0x80000000);
        REQUIRE(err == MemErr::None);
        REQUIRE_NOTHROW(interp.c_lwsp(cpu.reg(10), 2)); // offset = 2*4 = 8
        REQUIRE(cpu.reg(10) == 0xFFFFFFFF80000000);
    }

    SECTION("c.ldsp - load doubleword stack-pointer relative") {
        err = mem.store<uint64_t>(layout.stack_base + 24, 0x123456789ABCDEF0);
        REQUIRE(err == MemErr::None);
        REQUIRE_NOTHROW(interp.c_ldsp(cpu.reg(10), 3)); // offset = 3*8 = 24
        REQUIRE(cpu.reg(10) == 0x123456789ABCDEF0);
    }

    SECTION("c.swsp - store word stack-pointer relative") {
        cpu.reg(11) = 0xDEADBEEF;
        auto sp = cpu.reg(2).val();
        REQUIRE_NOTHROW(interp.c_swsp(cpu.reg(11), 2)); // imm=2, offset = 2*4 = 8
        auto val = mem.load<uint32_t>(sp + 8, err);
        REQUIRE(err == MemErr::None);
        REQUIRE(val == 0xDEADBEEF);
    }

    SECTION("c.sdsp - store doubleword stack-pointer relative") {
        cpu.reg(11) = 0xFEEDFACECAFEBABE;
        auto sp = cpu.reg(2).val();
        REQUIRE_NOTHROW(interp.c_sdsp(cpu.reg(11), 1)); // imm=1, offset = 1*8 = 8
        auto val = mem.load<uint64_t>(sp + 8, err);
        REQUIRE(err == MemErr::None);
        REQUIRE(val == 0xFEEDFACECAFEBABE);
    }
}

TEST_CASE("RV64C load/store with register offset", "[rv64c][memory]") {
    VM vm{};
    Interpreter interp{vm};
    auto &cpu = vm.m_cpu;
    Memory &mem = vm.m_memory;
    const auto &layout = vm.get_memory_layout();
    MemErr err = MemErr::None;

    // Use x8-x15 range (compressed registers)
    cpu.reg(8) = layout.data_base;

    SECTION("c.lw - load word") {
        err = mem.store<uint32_t>(layout.data_base + 12, 0xCAFEBABE);
        REQUIRE(err == MemErr::None);
        REQUIRE_NOTHROW(interp.c_lw(cpu.reg(9), cpu.reg(8), 3)); // offset = 3*4 = 12
        REQUIRE(cpu.reg(9).val() == 0xFFFFFFFFCAFEBABE);
    }

    SECTION("c.ld - load doubleword") {
        err = mem.store<uint64_t>(layout.data_base + 16, 0x0123456789ABCDEF);
        REQUIRE(err == MemErr::None);
        REQUIRE_NOTHROW(interp.c_ld(cpu.reg(9), cpu.reg(8), 2)); // offset = 2*8 = 16
        REQUIRE(cpu.reg(9) == 0x0123456789ABCDEF);
    }

    SECTION("c.sw - store word") {
        cpu.reg(9) = 0x12345678;
        REQUIRE_NOTHROW(interp.c_sw(cpu.reg(9), cpu.reg(8), 1)); // offset = 1*4 = 4
        auto val = mem.load<uint32_t>(layout.data_base + 4, err);
        REQUIRE(err == MemErr::None);
        REQUIRE(val == 0x12345678);
    }

    SECTION("c.sd - store doubleword") {
        cpu.reg(9) = 0xABCDEF0123456789;
        REQUIRE_NOTHROW(interp.c_sd(cpu.reg(9), cpu.reg(8), 1)); // offset = 1*8 = 8
        auto val = mem.load<uint64_t>(layout.data_base + 8, err);
        REQUIRE(err == MemErr::None);
        REQUIRE(val == 0xABCDEF0123456789);
    }
}

TEST_CASE("RV64C control transfer instructions", "[rv64c][jump][branch]") {
    VM vm{};
    Interpreter interp{vm};
    auto &cpu = vm.m_cpu;
    uint64_t initial_pc = 0x1000;

    SECTION("c.j - unconditional jump") {
        cpu.set_pc(initial_pc);
        interp.c_j(50); // offset = 50*2 = 100
        REQUIRE(cpu.get_pc() == initial_pc + 100);
    }

    SECTION("c.j - negative offset") {
        cpu.set_pc(initial_pc);
        interp.c_j(-20); // offset = -20*2 = -40
        REQUIRE(cpu.get_pc() == initial_pc - 40);
    }

    SECTION("c.jr - jump register") {
        cpu.set_pc(initial_pc);
        cpu.reg(5) = 0x2000;
        interp.c_jr(cpu.reg(5));
        REQUIRE(cpu.get_pc() == 0x2000);
    }

    SECTION("c.jalr - jump and link register") {
        cpu.set_pc(initial_pc);
        cpu.reg(5) = 0x3000;
        interp.c_jalr(cpu.reg(5));
        REQUIRE(cpu.get_pc() == 0x3000);
        REQUIRE(cpu.reg(1) == initial_pc); // ra = return address
    }

    SECTION("c.beqz - branch if equal zero") {
        cpu.set_pc(initial_pc);
        cpu.reg(8) = 0;
        interp.c_beqz(cpu.reg(8), 10); // offset = 10*2 = 20
        REQUIRE(cpu.get_pc() == initial_pc + 20);

        cpu.set_pc(initial_pc);
        cpu.reg(9) = 42;
        interp.c_beqz(cpu.reg(9), 10);
        REQUIRE(cpu.get_pc() == initial_pc);
    }

    SECTION("c.bnez - branch if not equal zero") {
        cpu.set_pc(initial_pc);
        cpu.reg(8) = 42;
        interp.c_bnez(cpu.reg(8), 10); // offset = 10*2 = 20
        REQUIRE(cpu.get_pc() == initial_pc + 20);

        cpu.set_pc(initial_pc);
        cpu.reg(9) = 0;
        interp.c_bnez(cpu.reg(9), 10);
        REQUIRE(cpu.get_pc() == initial_pc);
    }

    SECTION("c.beqz - negative offset") {
        cpu.set_pc(initial_pc);
        cpu.reg(8) = 0;
        interp.c_beqz(cpu.reg(8), -5); // offset = -5*2 = -10
        REQUIRE(cpu.get_pc() == initial_pc - 10);
    }
}

TEST_CASE("RV64C integer constant generation", "[rv64c][immediate]") {
    VM vm{};
    Interpreter interp{vm};
    auto &cpu = vm.m_cpu;

    SECTION("c.li - load immediate") {
        interp.c_li(cpu.reg(10), 31);
        REQUIRE(cpu.reg(10) == 31);

        interp.c_li(cpu.reg(11), -1);
        REQUIRE(cpu.reg(11) == UINT64_MAX);

        interp.c_li(cpu.reg(12), -32);
        REQUIRE(cpu.reg(12).sval() == -32);
    }

    SECTION("c.lui - load upper immediate") {
        interp.c_lui(cpu.reg(10), 1);
        REQUIRE(cpu.reg(10) == 0x1000);

        interp.c_lui(cpu.reg(11), 31);
        REQUIRE(cpu.reg(11) == 0x1F000);

        interp.c_lui(cpu.reg(12), -1);
        REQUIRE(cpu.reg(12) == 0xFFFFFFFFFFFFF000);
    }
}

TEST_CASE("RV64C integer register-immediate operations", "[rv64c][arithmetic][immediate]") {
    VM vm{};
    Interpreter interp{vm};
    auto &cpu = vm.m_cpu;

    SECTION("c.addi - add immediate") {
        cpu.reg(10) = 100;
        interp.c_addi(cpu.reg(10), 5);
        REQUIRE(cpu.reg(10) == 105);

        cpu.reg(11) = 100;
        interp.c_addi(cpu.reg(11), -10);
        REQUIRE(cpu.reg(11) == 90);
    }

    SECTION("c.addiw - add immediate word") {
        cpu.reg(10) = 0x7FFFFFFF;
        interp.c_addiw(cpu.reg(10), 1);
        REQUIRE(cpu.reg(10) == 0xFFFFFFFF80000000);

        cpu.reg(11) = 10;
        interp.c_addiw(cpu.reg(11), -5);
        REQUIRE(cpu.reg(11) == 5);
    }

    SECTION("c.addi16sp - add immediate to sp scaled by 16") {
        cpu.reg(2) = 0x1000;
        interp.c_addi16sp(cpu.reg(2), 2); // 2 * 16 = 32
        REQUIRE(cpu.reg(2) == 0x1020);

        cpu.reg(2) = 0x1000;
        interp.c_addi16sp(cpu.reg(2), -1); // -1 * 16 = -16
        REQUIRE(cpu.reg(2) == 0x0FF0);
    }

    SECTION("c.addi4spn - add immediate to sp scaled by 4") {
        cpu.reg(2) = 0x1000;
        interp.c_addi4spn(cpu.reg(8), 8); // 8 * 4 = 32
        REQUIRE(cpu.reg(8) == 0x1020);
    }

    SECTION("c.slli - shift left logical immediate") {
        cpu.reg(10) = 1;
        interp.c_slli(cpu.reg(10), 5);
        REQUIRE(cpu.reg(10) == 32);

        cpu.reg(11) = 0xFF;
        interp.c_slli(cpu.reg(11), 8);
        REQUIRE(cpu.reg(11) == 0xFF00);
    }

    SECTION("c.srli - shift right logical immediate") {
        cpu.reg(8) = 0x100;
        interp.c_srli(cpu.reg(8), 4);
        REQUIRE(cpu.reg(8) == 0x10);

        cpu.reg(9) = UINT64_MAX;
        interp.c_srli(cpu.reg(9), 1);
        REQUIRE(cpu.reg(9) == 0x7FFFFFFFFFFFFFFF);
    }

    SECTION("c.srai - shift right arithmetic immediate") {
        cpu.reg(8) = static_cast<uint64_t>(-64);
        interp.c_srai(cpu.reg(8), 2);
        REQUIRE(cpu.reg(8).sval() == -16);

        cpu.reg(9) = 0x80;
        interp.c_srai(cpu.reg(9), 4);
        REQUIRE(cpu.reg(9) == 0x08);
    }

    SECTION("c.andi - bitwise AND immediate") {
        cpu.reg(8) = 0xFF;
        interp.c_andi(cpu.reg(8), 0x0F);
        REQUIRE(cpu.reg(8) == 0x0F);

        cpu.reg(9) = 0xAAAA;
        interp.c_andi(cpu.reg(9), -1);
        REQUIRE(cpu.reg(9) == 0xAAAA);
    }
}

TEST_CASE("RV64C integer register-register operations", "[rv64c][arithmetic][register]") {
    VM vm{};
    Interpreter interp{vm};
    auto &cpu = vm.m_cpu;

    SECTION("c.mv - move register") {
        cpu.reg(5) = 0xDEADBEEF;
        interp.c_mv(cpu.reg(10), cpu.reg(5));
        REQUIRE(cpu.reg(10) == 0xDEADBEEF);
    }

    SECTION("c.add - add registers") {
        cpu.reg(10) = 100;
        cpu.reg(5) = 50;
        interp.c_add(cpu.reg(10), cpu.reg(5));
        REQUIRE(cpu.reg(10) == 150);
    }

    SECTION("c.and - bitwise AND") {
        cpu.reg(8) = 0xFF;
        cpu.reg(9) = 0x0F;
        interp.c_and(cpu.reg(8), cpu.reg(9));
        REQUIRE(cpu.reg(8) == 0x0F);
    }

    SECTION("c.or - bitwise OR") {
        cpu.reg(8) = 0xF0;
        cpu.reg(9) = 0x0F;
        interp.c_or(cpu.reg(8), cpu.reg(9));
        REQUIRE(cpu.reg(8) == 0xFF);
    }

    SECTION("c.xor - bitwise XOR") {
        cpu.reg(8) = 0xFF;
        cpu.reg(9) = 0xFF;
        interp.c_xor(cpu.reg(8), cpu.reg(9));
        REQUIRE(cpu.reg(8) == 0);
    }

    SECTION("c.sub - subtract registers") {
        cpu.reg(8) = 100;
        cpu.reg(9) = 30;
        interp.c_sub(cpu.reg(8), cpu.reg(9));
        REQUIRE(cpu.reg(8) == 70);
    }

    SECTION("c.addw - add word") {
        cpu.reg(8) = 0x7FFFFFFF;
        cpu.reg(9) = 1;
        interp.c_addw(cpu.reg(8), cpu.reg(9));
        REQUIRE(cpu.reg(8) == 0xFFFFFFFF80000000);
    }

    SECTION("c.subw - subtract word") {
        cpu.reg(8) = 0;
        cpu.reg(9) = 1;
        interp.c_subw(cpu.reg(8), cpu.reg(9));
        REQUIRE(cpu.reg(8) == 0xFFFFFFFFFFFFFFFF);
    }
}

TEST_CASE("RV64C x0 behavior", "[rv64c][register][zero]") {
    VM vm{};
    Interpreter interp{vm};
    auto &cpu = vm.m_cpu;

    SECTION("c.mv to x0 has no effect") {
        cpu.reg(5) = 999;
        interp.c_mv(cpu.reg(0), cpu.reg(5));
        REQUIRE(cpu.reg(0) == 0);
    }

    SECTION("c.add to x0 has no effect") {
        cpu.reg(5) = 100;
        interp.c_add(cpu.reg(0), cpu.reg(5));
        REQUIRE(cpu.reg(0) == 0);
    }

    SECTION("c.li to x0 has no effect") {
        interp.c_li(cpu.reg(0), 42);
        REQUIRE(cpu.reg(0) == 0);
    }
}

TEST_CASE("RV64C edge cases", "[rv64c][edge_cases]") {
    VM vm{};
    Interpreter interp{vm};
    auto &cpu = vm.m_cpu;

    SECTION("c.addi with zero immediate (NOP)") {
        cpu.reg(10) = 42;
        interp.c_addi(cpu.reg(10), 0);
        REQUIRE(cpu.reg(10) == 42);
    }

    SECTION("c.slli with maximum shift") {
        cpu.reg(10) = 1;
        interp.c_slli(cpu.reg(10), 63);
        REQUIRE(cpu.reg(10) == 0x8000000000000000);
    }

    SECTION("c.srli with maximum shift") {
        cpu.reg(8) = UINT64_MAX;
        interp.c_srli(cpu.reg(8), 63);
        REQUIRE(cpu.reg(8) == 1);
    }

    SECTION("Overflow in c.add") {
        cpu.reg(10) = UINT64_MAX;
        cpu.reg(5) = 1;
        interp.c_add(cpu.reg(10), cpu.reg(5));
        REQUIRE(cpu.reg(10) == 0);
    }

    SECTION("Underflow in c.sub") {
        cpu.reg(8) = 0;
        cpu.reg(9) = 1;
        interp.c_sub(cpu.reg(8), cpu.reg(9));
        REQUIRE(cpu.reg(8) == UINT64_MAX);
    }
}

