#include <catch2/catch_all.hpp>
#include <rv64/VM.hpp>
#include <rv64/Cpu.hpp>
#include <rv64/Interpreter.hpp>

using namespace rv64;

constexpr int INSTR_SIZE = 4;

TEST_CASE("RV64I arithmetic immediate instructions", "[rv64i][arithmetic][immediate]") {
    VM vm{};
    Interpreter interp{vm};
    auto &cpu = vm.m_cpu;

    SECTION("addi - add immediate") {
        cpu.reg(1) = 100;
        interp.addi(cpu.reg(2), cpu.reg(1), 50);
        REQUIRE(cpu.reg(2) == 150);

        cpu.reg(3) = 200;
        interp.addi(cpu.reg(4), cpu.reg(3), -50);
        REQUIRE(cpu.reg(4) == 150);

        cpu.reg(5) = UINT64_MAX;
        interp.addi(cpu.reg(6), cpu.reg(5), 1);
        REQUIRE(cpu.reg(6) == 0);
    }

    SECTION("slti - set less than immediate (signed)") {
        cpu.reg(1) = 10;
        interp.slti(cpu.reg(2), cpu.reg(1), 20);
        REQUIRE(cpu.reg(2) == 1);

        cpu.reg(3) = 30;
        interp.slti(cpu.reg(4), cpu.reg(3), 20);
        REQUIRE(cpu.reg(4) == 0);

        cpu.reg(5) = static_cast<uint64_t>(-10);
        interp.slti(cpu.reg(6), cpu.reg(5), 0);
        REQUIRE(cpu.reg(6) == 1);
    }

    SECTION("sltiu - set less than immediate unsigned") {
        cpu.reg(1) = 10;
        interp.sltiu(cpu.reg(2), cpu.reg(1), 20);
        REQUIRE(cpu.reg(2) == 1);

        cpu.reg(3) = 30;
        interp.sltiu(cpu.reg(4), cpu.reg(3), 20);
        REQUIRE(cpu.reg(4) == 0);

        cpu.reg(5) = UINT64_MAX;
        interp.sltiu(cpu.reg(6), cpu.reg(5), -1);
        REQUIRE(cpu.reg(6) == 0);
    }

    SECTION("andi - bitwise AND immediate") {
        cpu.reg(1) = 0xFF;
        interp.andi(cpu.reg(2), cpu.reg(1), 0x0F);
        REQUIRE(cpu.reg(2) == 0x0F);

        cpu.reg(3) = 0xAAAAAAAAAAAAAAAA;
        interp.andi(cpu.reg(4), cpu.reg(3), 0x555);
        REQUIRE(cpu.reg(4) == 0x000);
    }

    SECTION("ori - bitwise OR immediate") {
        cpu.reg(1) = 0xF0;
        interp.ori(cpu.reg(2), cpu.reg(1), 0x0F);
        REQUIRE(cpu.reg(2) == 0xFF);

        cpu.reg(3) = 0;
        interp.ori(cpu.reg(4), cpu.reg(3), -1);
        REQUIRE(cpu.reg(4) == UINT64_MAX);
    }

    SECTION("xori - bitwise XOR immediate") {
        cpu.reg(1) = 0xFF;
        interp.xori(cpu.reg(2), cpu.reg(1), 0xFF);
        REQUIRE(cpu.reg(2) == 0);

        cpu.reg(3) = 0xAAAAAAAAAAAAAAAA;
        interp.xori(cpu.reg(4), cpu.reg(3), -1);
        REQUIRE(cpu.reg(4) == 0x5555555555555555);
    }
}

TEST_CASE("RV64I shift immediate instructions", "[rv64i][shift][immediate]") {
    VM vm{};
    Interpreter interp{vm};
    auto &cpu = vm.m_cpu;

    SECTION("slli - shift left logical immediate") {
        cpu.reg(1) = 1;
        interp.slli(cpu.reg(2), cpu.reg(1), 5);
        REQUIRE(cpu.reg(2) == 32);

        cpu.reg(3) = 0xFF;
        interp.slli(cpu.reg(4), cpu.reg(3), 8);
        REQUIRE(cpu.reg(4) == 0xFF00);

        cpu.reg(5) = 0x12345678;
        interp.slli(cpu.reg(6), cpu.reg(5), 0);
        REQUIRE(cpu.reg(6) == 0x12345678);
    }

    SECTION("srli - shift right logical immediate") {
        cpu.reg(1) = 0x80;
        interp.srli(cpu.reg(2), cpu.reg(1), 3);
        REQUIRE(cpu.reg(2) == 0x10);

        cpu.reg(3) = 0xFF00;
        interp.srli(cpu.reg(4), cpu.reg(3), 8);
        REQUIRE(cpu.reg(4) == 0xFF);

        cpu.reg(5) = UINT64_MAX;
        interp.srli(cpu.reg(6), cpu.reg(5), 1);
        REQUIRE(cpu.reg(6) == 0x7FFFFFFFFFFFFFFF);
    }

    SECTION("srai - shift right arithmetic immediate") {
        cpu.reg(1) = 0x80;
        interp.srai(cpu.reg(2), cpu.reg(1), 3);
        REQUIRE(cpu.reg(2) == 0x10);

        cpu.reg(3) = static_cast<uint64_t>(-16);
        interp.srai(cpu.reg(4), cpu.reg(3), 2);
        REQUIRE(cpu.reg(4) == -4);

        cpu.reg(5) = 0x7FFFFFFFFFFFFFFF;
        interp.srai(cpu.reg(6), cpu.reg(5), 1);
        REQUIRE(cpu.reg(6) == 0x3FFFFFFFFFFFFFFF);
    }
}

TEST_CASE("RV64I upper immediate instructions", "[rv64i][upper][immediate]") {
    VM vm{};
    Interpreter interp{vm};
    auto &cpu = vm.m_cpu;

    SECTION("lui - load upper immediate") {
        interp.lui(cpu.reg(1), 0x12345);
        REQUIRE(cpu.reg(1) == 0x12345000);

        interp.lui(cpu.reg(2), -1);
        REQUIRE((cpu.reg(2) == UINT64_C(0xFFFFFFFFFFFFF000)));
    }

    SECTION("auipc - add upper immediate to PC") {
        cpu.set_pc(0x1000);
        interp.auipc(cpu.reg(1), 0x10);
        REQUIRE(cpu.reg(1) == 0x1000 + 0x10000);

        cpu.set_pc(0x80000000);
        interp.auipc(cpu.reg(2), 0x1);
        REQUIRE(cpu.reg(2) == 0x80000000 + 0x1000);
    }
}

TEST_CASE("RV64I arithmetic register instructions", "[rv64i][arithmetic][register]") {
    VM vm{};
    Interpreter interp{vm};
    auto &cpu = vm.m_cpu;

    SECTION("add - add registers") {
        cpu.reg(1) = 100;
        cpu.reg(2) = 50;
        interp.add(cpu.reg(3), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(3) == 150);

        cpu.reg(4) = UINT64_MAX;
        cpu.reg(5) = 1;
        interp.add(cpu.reg(6), cpu.reg(4), cpu.reg(5));
        REQUIRE(cpu.reg(6) == 0);
    }

    SECTION("sub - subtract registers") {
        cpu.reg(1) = 100;
        cpu.reg(2) = 50;
        interp.sub(cpu.reg(3), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(3) == 50);

        cpu.reg(4) = 0;
        cpu.reg(5) = 1;
        interp.sub(cpu.reg(6), cpu.reg(4), cpu.reg(5));
        REQUIRE(cpu.reg(6) == UINT64_MAX);
    }

    SECTION("slt - set less than (signed)") {
        cpu.reg(1) = 10;
        cpu.reg(2) = 20;
        interp.slt(cpu.reg(3), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(3) == 1);

        cpu.reg(4) = 30;
        cpu.reg(5) = 20;
        interp.slt(cpu.reg(6), cpu.reg(4), cpu.reg(5));
        REQUIRE(cpu.reg(6) == 0);

        cpu.reg(7) = static_cast<uint64_t>(-10);
        cpu.reg(8) = 10;
        interp.slt(cpu.reg(9), cpu.reg(7), cpu.reg(8));
        REQUIRE(cpu.reg(9) == 1);
    }

    SECTION("sltu - set less than unsigned") {
        cpu.reg(1) = 10;
        cpu.reg(2) = 20;
        interp.sltu(cpu.reg(3), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(3) == 1);

        cpu.reg(4) = static_cast<uint64_t>(-10);
        cpu.reg(5) = 10;
        interp.sltu(cpu.reg(6), cpu.reg(4), cpu.reg(5));
        REQUIRE(cpu.reg(6) == 0);
    }
}

TEST_CASE("RV64I bitwise register instructions", "[rv64i][bitwise][register]") {
    VM vm{};
    Interpreter interp{vm};
    auto &cpu = vm.m_cpu;

    SECTION("and - bitwise AND") {
        cpu.reg(1) = 0xFF;
        cpu.reg(2) = 0xF0;
        interp.and_(cpu.reg(3), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(3) == 0xF0);
    }

    SECTION("or - bitwise OR") {
        cpu.reg(1) = 0xF0;
        cpu.reg(2) = 0x0F;
        interp.or_(cpu.reg(3), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(3) == 0xFF);
    }

    SECTION("xor - bitwise XOR") {
        cpu.reg(1) = 0xFF;
        cpu.reg(2) = 0xFF;
        interp.xor_(cpu.reg(3), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(3) == 0);

        cpu.reg(4) = 0xAAAAAAAAAAAAAAAA;
        cpu.reg(5) = UINT64_MAX;
        interp.xor_(cpu.reg(6), cpu.reg(4), cpu.reg(5));
        REQUIRE(cpu.reg(6) == 0x5555555555555555);
    }
}

TEST_CASE("RV64I shift register instructions", "[rv64i][shift][register]") {
    VM vm{};
    Interpreter interp{vm};
    auto &cpu = vm.m_cpu;

    SECTION("sll - shift left logical") {
        cpu.reg(1) = 1;
        cpu.reg(2) = 5;
        interp.sll(cpu.reg(3), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(3) == 32);

        cpu.reg(4) = 0xFF;
        cpu.reg(5) = 64 + 4;
        interp.sll(cpu.reg(6), cpu.reg(4), cpu.reg(5));
        REQUIRE(cpu.reg(6) == 0xFF0);
    }

    SECTION("srl - shift right logical") {
        cpu.reg(1) = 0x80;
        cpu.reg(2) = 3;
        interp.srl(cpu.reg(3), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(3) == 0x10);

        cpu.reg(4) = UINT64_MAX;
        cpu.reg(5) = 1;
        interp.srl(cpu.reg(6), cpu.reg(4), cpu.reg(5));
        REQUIRE(cpu.reg(6) == 0x7FFFFFFFFFFFFFFF);
    }

    SECTION("sra - shift right arithmetic") {
        cpu.reg(1) = 0x80;
        cpu.reg(2) = 3;
        interp.sra(cpu.reg(3), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(3) == 0x10);

        cpu.reg(4) = static_cast<uint64_t>(-16);
        cpu.reg(5) = 2;
        interp.sra(cpu.reg(6), cpu.reg(4), cpu.reg(5));
        REQUIRE(cpu.reg(6) == -4);
    }
}

TEST_CASE("RV64I 32-bit arithmetic instructions", "[rv64i][arithmetic][32bit]") {
    VM vm{};
    Interpreter interp{vm};
    auto &cpu = vm.m_cpu;

    SECTION("addiw - add immediate word") {
        cpu.reg(1).val() = 0x00000000FFFFFFFF;
        interp.addiw(cpu.reg(2), cpu.reg(1), 1);
        REQUIRE(cpu.reg(2) == 0);

        cpu.reg(3) = 0x000000007FFFFFFF;
        interp.addiw(cpu.reg(4), cpu.reg(3), 1);
        auto val = cpu.reg(4).val();
        REQUIRE(val == 0xFFFFFFFF80000000);
    }

    SECTION("addw - add word") {
        cpu.reg(1).val() = 0x00000000FFFFFFFF;
        cpu.reg(2) = 1;
        interp.addw(cpu.reg(3), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(3) == 0);

        cpu.reg(4) = 0x000000007FFFFFFF;
        cpu.reg(5) = 1;
        interp.addw(cpu.reg(6), cpu.reg(4), cpu.reg(5));
        REQUIRE(cpu.reg(6) == 0xFFFFFFFF80000000);
    }

    SECTION("subw - subtract word") {
        cpu.reg(1) = 0;
        cpu.reg(2) = 1;
        interp.subw(cpu.reg(3), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(3) == 0xFFFFFFFFFFFFFFFF);
    }

    SECTION("slliw - shift left logical immediate word") {
        cpu.reg(1) = 1;
        interp.slliw(cpu.reg(2), cpu.reg(1), 5);
        REQUIRE(cpu.reg(2) == 32);

        cpu.reg(3) = 0x40000000;
        interp.slliw(cpu.reg(4), cpu.reg(3), 1);
        REQUIRE(cpu.reg(4) == 0xFFFFFFFF80000000);
    }

    SECTION("srliw - shift right logical immediate word") {
        cpu.reg(1) = 0xFFFFFFFF80000000;
        interp.srliw(cpu.reg(2), cpu.reg(1), 1);
        REQUIRE(cpu.reg(2) == 0x0000000040000000);
    }

    SECTION("sraiw - shift right arithmetic immediate word") {
        cpu.reg(1) = 0xFFFFFFFF80000000;
        interp.sraiw(cpu.reg(2), cpu.reg(1), 1);
        REQUIRE(cpu.reg(2) == 0xFFFFFFFFC0000000);
    }

    SECTION("sllw - shift left logical word") {
        cpu.reg(1) = 1;
        cpu.reg(2) = 5;
        interp.sllw(cpu.reg(3), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(3) == 32);
    }

    SECTION("srlw - shift right logical word") {
        cpu.reg(1) = 0xFFFFFFFF80000000;
        cpu.reg(2) = 1;
        interp.srlw(cpu.reg(3), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(3) == 0x0000000040000000);
    }

    SECTION("sraw - shift right arithmetic word") {
        cpu.reg(1) = 0xFFFFFFFF80000000;
        cpu.reg(2) = 1;
        interp.sraw(cpu.reg(3), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(3) == 0xFFFFFFFFC0000000);
    }
}

TEST_CASE("RV64I load/store instructions", "[rv64i][memory][load][store]") {
    VM vm{};
    Interpreter interp{vm};
    auto &cpu = vm.m_cpu;
    Memory &mem = vm.m_memory;
    const auto &layout = vm.get_memory_layout();
    MemErr err = MemErr::None;

    SECTION("lb - load byte signed") {
        err = mem.store<uint8_t>(layout.data_base, 0x80);
        REQUIRE(err == MemErr::None);
        cpu.reg(1) = layout.data_base;
        REQUIRE_NOTHROW(interp.lb(cpu.reg(2), cpu.reg(1), 0));
        REQUIRE(cpu.reg(2) == 0xFFFFFFFFFFFFFF80);
    }

    SECTION("lbu - load byte unsigned") {
        err = mem.store<uint8_t>(layout.data_base, 0x80);
        REQUIRE(err == MemErr::None);
        cpu.reg(1) = layout.data_base;
        REQUIRE_NOTHROW(interp.lbu(cpu.reg(2), cpu.reg(1), 0));
        REQUIRE(cpu.reg(2) == 0x80);
    }

    SECTION("lh - load halfword signed") {
        err = mem.store<uint16_t>(layout.data_base, 0x8000);
        REQUIRE(err == MemErr::None);
        cpu.reg(1) = layout.data_base;
        REQUIRE_NOTHROW(interp.lh(cpu.reg(2), cpu.reg(1), 0));
        REQUIRE(cpu.reg(2) == 0xFFFFFFFFFFFF8000);
    }

    SECTION("lhu - load halfword unsigned") {
        err = mem.store<uint16_t>(layout.data_base, 0x8000);
        REQUIRE(err == MemErr::None);
        cpu.reg(1) = layout.data_base;
        REQUIRE_NOTHROW(interp.lhu(cpu.reg(2), cpu.reg(1), 0));
        REQUIRE(cpu.reg(2) == 0x8000);
    }

    SECTION("lw - load word signed") {
        err = mem.store<uint32_t>(layout.data_base, 0x80000000);
        REQUIRE(err == MemErr::None);
        cpu.reg(1) = layout.data_base;
        REQUIRE_NOTHROW(interp.lw(cpu.reg(2), cpu.reg(1), 0));
        REQUIRE(cpu.reg(2) == 0xFFFFFFFF80000000);
    }

    SECTION("lwu - load word unsigned") {
        err = mem.store<uint32_t>(layout.data_base, 0x80000000);
        REQUIRE(err == MemErr::None);
        cpu.reg(1) = layout.data_base;
        REQUIRE_NOTHROW(interp.lwu(cpu.reg(2), cpu.reg(1), 0));
        REQUIRE(cpu.reg(2) == 0x80000000);
    }

    SECTION("ld - load doubleword") {
        err = mem.store<uint64_t>(layout.data_base, 0x123456789ABCDEF0);
        REQUIRE(err == MemErr::None);
        cpu.reg(1) = layout.data_base;
        REQUIRE_NOTHROW(interp.ld(cpu.reg(2), cpu.reg(1), 0));
        REQUIRE(cpu.reg(2) == 0x123456789ABCDEF0);
    }

    SECTION("sb - store byte") {
        cpu.reg(1) = layout.data_base;
        cpu.reg(2) = 0xFFFFFFFFFFFFFF42;
        REQUIRE_NOTHROW(interp.sb(cpu.reg(2), cpu.reg(1), 0));

        auto val = mem.load<uint8_t>(layout.data_base, err);
        REQUIRE(err == MemErr::None);
        REQUIRE(val == 0x42);
    }

    SECTION("sh - store halfword") {
        cpu.reg(1) = layout.data_base;
        cpu.reg(2) = 0xFFFFFFFFFFFF1234;
        REQUIRE_NOTHROW(interp.sh(cpu.reg(2), cpu.reg(1), 0));

        auto val = mem.load<uint16_t>(layout.data_base, err);
        REQUIRE(err == MemErr::None);
        REQUIRE(val == 0x1234);
    }

    SECTION("sw - store word") {
        cpu.reg(1) = layout.data_base;
        cpu.reg(2) = 0xFFFFFFFF12345678;
        REQUIRE_NOTHROW(interp.sw(cpu.reg(2), cpu.reg(1), 0));

        auto val = mem.load<uint32_t>(layout.data_base, err);
        REQUIRE(err == MemErr::None);
        REQUIRE(val == 0x12345678);
    }

    SECTION("sd - store doubleword") {
        cpu.reg(1) = layout.data_base;
        cpu.reg(2) = 0x123456789ABCDEF0;
        REQUIRE_NOTHROW(interp.sd(cpu.reg(2), cpu.reg(1), 0));

        auto val = mem.load<uint64_t>(layout.data_base, err);
        REQUIRE(err == MemErr::None);
        REQUIRE(val == 0x123456789ABCDEF0);
    }

    SECTION("Load/store with offset") {
        cpu.reg(1) = layout.data_base;
        cpu.reg(2).val() = 0xDEADBEEF;

        REQUIRE_NOTHROW(interp.sw(cpu.reg(2), cpu.reg(1), 8));
        REQUIRE_NOTHROW(interp.lw(cpu.reg(3), cpu.reg(1), 8));

        REQUIRE(cpu.reg(3) == 0xFFFFFFFFDEADBEEF);
    }
}

TEST_CASE("RV64I branch instructions", "[rv64i][branch]") {
    VM vm{};
    Interpreter interp{vm};
    auto &cpu = vm.m_cpu;
    uint64_t initial_pc = 0x1000;

    SECTION("beq - branch if equal") {
        cpu.set_pc(initial_pc);
        cpu.reg(1) = 100;
        cpu.reg(2) = 100;
        interp.beq(cpu.reg(1), cpu.reg(2), 8);
        REQUIRE(cpu.get_pc() == initial_pc + 16 - INSTR_SIZE);

        cpu.set_pc(initial_pc);
        cpu.reg(3) = 100;
        cpu.reg(4) = 200;
        interp.beq(cpu.reg(3), cpu.reg(4), 8);
        REQUIRE(cpu.get_pc() == initial_pc);
    }

    SECTION("bne - branch if not equal") {
        cpu.set_pc(initial_pc);
        cpu.reg(1) = 100;
        cpu.reg(2) = 200;
        interp.bne(cpu.reg(1), cpu.reg(2), 8);
        REQUIRE(cpu.get_pc() == initial_pc + 16 - INSTR_SIZE);

        cpu.set_pc(initial_pc);
        cpu.reg(3) = 100;
        cpu.reg(4) = 100;
        interp.bne(cpu.reg(3), cpu.reg(4), 8);
        REQUIRE(cpu.get_pc() == initial_pc);
    }

    SECTION("blt - branch if less than (signed)") {
        cpu.set_pc(initial_pc);
        cpu.reg(1) = 10;
        cpu.reg(2) = 20;
        interp.blt(cpu.reg(1), cpu.reg(2), 8);
        REQUIRE(cpu.get_pc() == initial_pc + 16 - INSTR_SIZE);

        cpu.set_pc(initial_pc);
        cpu.reg(3) = static_cast<uint64_t>(-10);
        cpu.reg(4) = 10;
        interp.blt(cpu.reg(3), cpu.reg(4), 8);
        REQUIRE(cpu.get_pc() == initial_pc + 16 - INSTR_SIZE);
    }

    SECTION("bltu - branch if less than unsigned") {
        cpu.set_pc(initial_pc);
        cpu.reg(1) = 10;
        cpu.reg(2) = 20;
        interp.bltu(cpu.reg(1), cpu.reg(2), 8);
        REQUIRE(cpu.get_pc() == initial_pc + 16 - INSTR_SIZE);

        cpu.set_pc(initial_pc);
        cpu.reg(3) = static_cast<uint64_t>(-10);
        cpu.reg(4) = 10;
        interp.bltu(cpu.reg(3), cpu.reg(4), 8);
        REQUIRE(cpu.get_pc() == initial_pc);
    }

    SECTION("bge - branch if greater or equal (signed)") {
        cpu.set_pc(initial_pc);
        cpu.reg(1) = 20;
        cpu.reg(2) = 10;
        interp.bge(cpu.reg(1), cpu.reg(2), 8);
        REQUIRE(cpu.get_pc() == initial_pc + 16 - INSTR_SIZE);

        cpu.set_pc(initial_pc);
        cpu.reg(3) = 10;
        cpu.reg(4) = 10;
        interp.bge(cpu.reg(3), cpu.reg(4), 8);
        REQUIRE(cpu.get_pc() == initial_pc + 16 - INSTR_SIZE);
    }

    SECTION("bgeu - branch if greater or equal unsigned") {
        cpu.set_pc(initial_pc);
        cpu.reg(1) = 20;
        cpu.reg(2) = 10;
        interp.bgeu(cpu.reg(1), cpu.reg(2), 8);
        REQUIRE(cpu.get_pc() == initial_pc + 16 - INSTR_SIZE);

        cpu.set_pc(initial_pc);
        cpu.reg(3) = static_cast<uint64_t>(-10);
        cpu.reg(4) = 10;
        interp.bgeu(cpu.reg(3), cpu.reg(4), 8);
        REQUIRE(cpu.get_pc() == initial_pc + 16 - INSTR_SIZE);
    }

    SECTION("Branch with negative offset") {
        cpu.set_pc(initial_pc);
        cpu.reg(1) = 100;
        cpu.reg(2) = 100;
        interp.beq(cpu.reg(1), cpu.reg(2), -8);
        REQUIRE(cpu.get_pc() == initial_pc - 16 - INSTR_SIZE);
    }
}

TEST_CASE("RV64I jump instructions", "[rv64i][jump]") {
    VM vm{};
    Interpreter interp{vm};
    auto &cpu = vm.m_cpu;
    uint64_t initial_pc = 0x1000;

    SECTION("jal - jump and link") {
        cpu.set_pc(initial_pc);
        interp.jal(cpu.reg(1), 100);
        REQUIRE(cpu.get_pc() == initial_pc + 200 - INSTR_SIZE);
        REQUIRE(cpu.reg(1) == initial_pc);
    }

    SECTION("jalr - jump and link register") {
        cpu.set_pc(initial_pc);
        cpu.reg(2) = 0x2000;
        interp.jalr(cpu.reg(1), cpu.reg(2), 8);
        REQUIRE(cpu.get_pc() == 0x2000 + 8);
        REQUIRE(cpu.reg(1) == initial_pc);
    }

    SECTION("jalr clears lowest bit") {
        cpu.set_pc(initial_pc);
        cpu.reg(2) = 0x2001;
        interp.jalr(cpu.reg(1), cpu.reg(2), 0);
        REQUIRE(cpu.get_pc() == 0x2000);
    }
}

TEST_CASE("RV64I register x0 is always zero", "[rv64i][register][zero]") {
    VM vm{};
    Interpreter interp{vm};
    auto &cpu = vm.m_cpu;

    SECTION("Writing to x0 has no effect") {
        cpu.reg(0) = 0;
        interp.addi(cpu.reg(0), cpu.reg(0), 100);
        REQUIRE(cpu.reg(0) == 0);

        interp.lui(cpu.reg(0), 0x12345);
        REQUIRE(cpu.reg(0) == 0);

        cpu.reg(1) = 999;
        interp.add(cpu.reg(0), cpu.reg(1), cpu.reg(1));
        REQUIRE(cpu.reg(0) == 0);
    }

    SECTION("Reading from x0 always returns zero") {
        cpu.reg(1) = 100;
        interp.add(cpu.reg(2), cpu.reg(0), cpu.reg(1));
        REQUIRE(cpu.reg(2) == 100);

        interp.or_(cpu.reg(3), cpu.reg(0), cpu.reg(0));
        REQUIRE(cpu.reg(3) == 0);
    }
}

TEST_CASE("RV64I sign extension behavior", "[rv64i][sign_extension]") {
    VM vm{};
    Interpreter interp{vm};
    auto &cpu = vm.m_cpu;
    Memory &mem = vm.m_memory;
    const auto &layout = vm.get_memory_layout();
    MemErr err = MemErr::None;

    SECTION("Load instructions properly sign-extend") {
        err = mem.store<uint8_t>(layout.data_base, 0x80);
        REQUIRE(err == MemErr::None);
        cpu.reg(1) = layout.data_base;
        REQUIRE_NOTHROW(interp.lb(cpu.reg(2), cpu.reg(1), 0));
        REQUIRE(cpu.reg(2) == 0xFFFFFFFFFFFFFF80);

        err = mem.store<uint16_t>(layout.data_base + 8, 0x8000);
        REQUIRE(err == MemErr::None);
        REQUIRE_NOTHROW(interp.lh(cpu.reg(3), cpu.reg(1), 8));
        REQUIRE(cpu.reg(3) == 0xFFFFFFFFFFFF8000);

        err = mem.store<uint32_t>(layout.data_base + 16, 0x80000000);
        REQUIRE(err == MemErr::None);
        REQUIRE_NOTHROW(interp.lw(cpu.reg(4), cpu.reg(1), 16));
        REQUIRE(cpu.reg(4) == 0xFFFFFFFF80000000);
    }

    SECTION("Load instructions properly zero-extend") {
        err = mem.store<uint8_t>(layout.data_base, 0xFF);
        REQUIRE(err == MemErr::None);
        cpu.reg(1) = layout.data_base;
        REQUIRE_NOTHROW(interp.lbu(cpu.reg(2), cpu.reg(1), 0));
        REQUIRE(cpu.reg(2) == 0xFF);

        err = mem.store<uint16_t>(layout.data_base + 8, 0xFFFF);
        REQUIRE(err == MemErr::None);
        REQUIRE_NOTHROW(interp.lhu(cpu.reg(3), cpu.reg(1), 8));
        REQUIRE(cpu.reg(3) == 0xFFFF);

        err = mem.store<uint32_t>(layout.data_base + 16, 0xFFFFFFFF);
        REQUIRE(err == MemErr::None);
        REQUIRE_NOTHROW(interp.lwu(cpu.reg(4), cpu.reg(1), 16));
        REQUIRE(cpu.reg(4) == 0xFFFFFFFF);
    }

    SECTION("32-bit operations sign-extend results") {
        cpu.reg(1) = 0xFFFFFFFF7FFFFFFF;
        interp.addiw(cpu.reg(2), cpu.reg(1), 1);
        REQUIRE(cpu.reg(2) == 0xFFFFFFFF80000000);

        cpu.reg(3) = 0xFFFFFFFF00000001;
        cpu.reg(4) = 0xFFFFFFFF00000001;
        interp.addw(cpu.reg(5), cpu.reg(3), cpu.reg(4));
        REQUIRE(cpu.reg(5) == 2);
    }
}

TEST_CASE("RV64I immediate sign extension", "[rv64i][immediate]") {
    VM vm{};
    Interpreter interp{vm};
    auto &cpu = vm.m_cpu;

    SECTION("12-bit immediate sign extends correctly") {
        cpu.reg(1) = 0;
        interp.addi(cpu.reg(2), cpu.reg(1), -1);
        REQUIRE(cpu.reg(2) == UINT64_MAX);

        cpu.reg(3) = 100;
        interp.addi(cpu.reg(4), cpu.reg(3), -50);
        REQUIRE(cpu.reg(4) == 50);
    }

    SECTION("20-bit immediate in lui") {
        interp.lui(cpu.reg(1), 0xFFFFF);
        REQUIRE(cpu.reg(1) == UINT64_C(0xFFFFFFFFFFFFF000));

        interp.lui(cpu.reg(2), 0x7FFFF);
        REQUIRE(cpu.reg(2) == 0x7FFFF000);
    }
}

TEST_CASE("RV64I edge cases", "[rv64i][edge_cases]") {
    VM vm{};
    Interpreter interp{vm};
    auto &cpu = vm.m_cpu;

    SECTION("Shift amount masking") {
        cpu.reg(1) = 1;
        cpu.reg(2) = 64 + 5;
        interp.sll(cpu.reg(3), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(3) == 32);

        cpu.reg(4) = 1;
        cpu.reg(5) = 32 + 3;
        interp.sllw(cpu.reg(6), cpu.reg(4), cpu.reg(5));
        REQUIRE(cpu.reg(6) == 8);
    }

    SECTION("Comparison with equal values") {
        cpu.reg(1) = 100;
        cpu.reg(2) = 100;

        interp.slt(cpu.reg(3), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(3) == 0);

        interp.sltu(cpu.reg(4), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(4) == 0);

        interp.slti(cpu.reg(5), cpu.reg(1), 100);
        REQUIRE(cpu.reg(5) == 0);

        interp.sltiu(cpu.reg(6), cpu.reg(1), 100);
        REQUIRE(cpu.reg(6) == 0);
    }

    SECTION("Zero in arithmetic operations") {
        cpu.reg(1) = 0;
        cpu.reg(2) = 100;

        interp.add(cpu.reg(3), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(3) == 100);

        interp.sub(cpu.reg(4), cpu.reg(2), cpu.reg(1));
        REQUIRE(cpu.reg(4) == 100);

        interp.and_(cpu.reg(6), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(6) == 0);

        interp.or_(cpu.reg(7), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(7) == 100);
    }

    SECTION("Maximum values") {
        cpu.reg(1) = UINT64_MAX;
        cpu.reg(2) = UINT64_MAX;

        interp.and_(cpu.reg(3), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(3) == UINT64_MAX);

        interp.or_(cpu.reg(4), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(4) == UINT64_MAX);

        interp.xor_(cpu.reg(5), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(5) == 0);

        interp.add(cpu.reg(6), cpu.reg(1), cpu.reg(2));
        REQUIRE(cpu.reg(6) == UINT64_MAX - 1);
    }
}

TEST_CASE("RV64I PC manipulation", "[rv64i][pc]") {
    VM vm{};
    Interpreter interp{vm};
    auto &cpu = vm.m_cpu;
    uint64_t initial_pc = 0x1000;

    SECTION("Branch instructions modify PC when condition is true") {
        cpu.set_pc(initial_pc);
        cpu.reg(1) = 100;
        cpu.reg(2) = 100;

        uint64_t pc_before = cpu.get_pc();
        interp.beq(cpu.reg(1), cpu.reg(2), 16);
        REQUIRE(cpu.get_pc() == pc_before + 32 - INSTR_SIZE);
    }

    SECTION("Branch instructions don't modify PC when condition is false") {
        cpu.set_pc(initial_pc);
        cpu.reg(1) = 100;
        cpu.reg(2) = 200;

        uint64_t pc_before = cpu.get_pc();
        interp.beq(cpu.reg(1), cpu.reg(2), 16);
        REQUIRE(cpu.get_pc() == pc_before);
    }

    SECTION("jal modifies PC and stores return address") {
        cpu.set_pc(initial_pc);
        interp.jal(cpu.reg(1), 100);
        REQUIRE(cpu.get_pc() == initial_pc + 200 - INSTR_SIZE);
        REQUIRE(cpu.reg(1) == initial_pc);
    }

    SECTION("jalr modifies PC to computed address") {
        cpu.set_pc(initial_pc);
        cpu.reg(2) = 0x5000;
        interp.jalr(cpu.reg(1), cpu.reg(2), 8);
        REQUIRE(cpu.get_pc() == (0x5000 + 8));
        REQUIRE(cpu.reg(1) == initial_pc);
    }

    SECTION("auipc computes address relative to PC") {
        cpu.set_pc(0x10000);
        interp.auipc(cpu.reg(1), 1);
        REQUIRE(cpu.reg(1) == 0x10000 + 0x1000);
        REQUIRE(cpu.get_pc() == 0x10000);
    }
}

TEST_CASE("RV64I fence and system instructions", "[rv64i][system]") {
    VM vm{};
    Interpreter interp{vm};
    auto &cpu = vm.m_cpu;

    SECTION("fence does nothing but shouldn't crash") {
        REQUIRE_NOTHROW(interp.fence());
    }

    SECTION("ebreak should trigger breakpoint") {
        vm.load_program({});
        REQUIRE(vm.get_state() == VMState::Loaded);

        REQUIRE_NOTHROW(interp.ebreak());
        REQUIRE(vm.get_state() == VMState::Breakpoint);
    }

    SECTION("ecall with a0=10 should exit program") {
        vm.load_program({});
        REQUIRE(vm.get_state() == VMState::Loaded);

        cpu.reg(10) = 10;  // a0 = 10 (exit syscall)
        REQUIRE_NOTHROW(interp.ecall());
        REQUIRE(vm.get_state() == VMState::Finished);
    }

}

