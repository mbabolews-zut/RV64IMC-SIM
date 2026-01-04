#include <catch2/catch_test_macros.hpp>
#include <parser/asm_parsing.hpp>
#include <rv64/VM.hpp>

// Initialize instruction table once for all tests
[[maybe_unused]] static rv64::VM vm;

TEST_CASE("Parser - R-type instructions", "[parser][r-type]") {
    asm_parsing::ParsedInstVec out;

    SECTION("add instruction") {
        int result = asm_parsing::parse_and_resolve("add x1, x2, x3", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "add");
    }

    SECTION("sub instruction") {
        int result = asm_parsing::parse_and_resolve("sub x10, x11, x12", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "sub");
    }

    SECTION("and instruction") {
        int result = asm_parsing::parse_and_resolve("and x5, x6, x7", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "and");
    }

    SECTION("or instruction") {
        int result = asm_parsing::parse_and_resolve("or x8, x9, x10", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "or");
    }

    SECTION("xor instruction") {
        int result = asm_parsing::parse_and_resolve("xor x1, x2, x3", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "xor");
    }

    SECTION("sll instruction") {
        int result = asm_parsing::parse_and_resolve("sll x1, x2, x3", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "sll");
    }

    SECTION("srl instruction") {
        int result = asm_parsing::parse_and_resolve("srl x1, x2, x3", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "srl");
    }

    SECTION("sra instruction") {
        int result = asm_parsing::parse_and_resolve("sra x1, x2, x3", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "sra");
    }
}

TEST_CASE("Parser - I-type instructions", "[parser][i-type]") {
    asm_parsing::ParsedInstVec out;

    SECTION("addi instruction") {
        int result = asm_parsing::parse_and_resolve("addi x1, x2, 100", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "addi");
    }

    SECTION("addi with negative immediate") {
        int result = asm_parsing::parse_and_resolve("addi x1, x2, -50", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "addi");
    }

    SECTION("andi instruction") {
        int result = asm_parsing::parse_and_resolve("andi x1, x2, 0xFF", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "andi");
    }

    SECTION("ori instruction") {
        int result = asm_parsing::parse_and_resolve("ori x1, x2, 0x10", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "ori");
    }

    SECTION("xori instruction") {
        int result = asm_parsing::parse_and_resolve("xori x1, x2, 123", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "xori");
    }

    SECTION("slti instruction") {
        int result = asm_parsing::parse_and_resolve("slti x1, x2, 10", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "slti");
    }

    SECTION("sltiu instruction") {
        int result = asm_parsing::parse_and_resolve("sltiu x1, x2, 10", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "sltiu");
    }
}

TEST_CASE("Parser - Load instructions", "[parser][load]") {
    asm_parsing::ParsedInstVec out;

    SECTION("lb instruction") {
        int result = asm_parsing::parse_and_resolve("lb x1, 0(x2)", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "lb");
    }

    SECTION("lh instruction") {
        int result = asm_parsing::parse_and_resolve("lh x1, 4(x2)", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "lh");
    }

    SECTION("lw instruction") {
        int result = asm_parsing::parse_and_resolve("lw x1, 8(x2)", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "lw");
    }

    SECTION("ld instruction") {
        int result = asm_parsing::parse_and_resolve("ld x1, 16(x2)", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "ld");
    }

    SECTION("lbu instruction") {
        int result = asm_parsing::parse_and_resolve("lbu x1, 0(x2)", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "lbu");
    }

    SECTION("lhu instruction") {
        int result = asm_parsing::parse_and_resolve("lhu x1, 2(x2)", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "lhu");
    }

    SECTION("lwu instruction") {
        int result = asm_parsing::parse_and_resolve("lwu x1, 4(x2)", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "lwu");
    }
}

TEST_CASE("Parser - Store instructions", "[parser][store]") {
    asm_parsing::ParsedInstVec out;

    SECTION("sb instruction") {
        int result = asm_parsing::parse_and_resolve("sb x1, 0(x2)", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "sb");
    }

    SECTION("sh instruction") {
        int result = asm_parsing::parse_and_resolve("sh x1, 2(x2)", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "sh");
    }

    SECTION("sw instruction") {
        int result = asm_parsing::parse_and_resolve("sw x1, 4(x2)", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "sw");
    }

    SECTION("sd instruction") {
        int result = asm_parsing::parse_and_resolve("sd x1, 8(x2)", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "sd");
    }
}

TEST_CASE("Parser - Branch instructions", "[parser][branch]") {
    asm_parsing::ParsedInstVec out;

    SECTION("beq with label") {
        int result = asm_parsing::parse_and_resolve("loop:\n  beq x1, x2, loop", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
    }

    SECTION("bne with label") {
        int result = asm_parsing::parse_and_resolve("start:\n  bne x1, x2, start", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
    }

    SECTION("blt with label") {
        int result = asm_parsing::parse_and_resolve("target:\n  blt x1, x2, target", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
    }

    SECTION("bge with label") {
        int result = asm_parsing::parse_and_resolve("end:\n  bge x1, x2, end", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
    }

    SECTION("bltu with label") {
        int result = asm_parsing::parse_and_resolve("lbl:\n  bltu x1, x2, lbl", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
    }

    SECTION("bgeu with label") {
        int result = asm_parsing::parse_and_resolve("done:\n  bgeu x1, x2, done", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
    }
}

TEST_CASE("Parser - U-type instructions", "[parser][u-type]") {
    asm_parsing::ParsedInstVec out;

    SECTION("lui instruction") {
        int result = asm_parsing::parse_and_resolve("lui x1, 0x12345", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "lui");
    }

    SECTION("auipc instruction") {
        int result = asm_parsing::parse_and_resolve("auipc x1, 0x1000", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "auipc");
    }
}

TEST_CASE("Parser - Jump instructions", "[parser][jump]") {
    asm_parsing::ParsedInstVec out;

    SECTION("jal with label") {
        int result = asm_parsing::parse_and_resolve("func:\n  jal x1, func", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
    }

    SECTION("jalr instruction") {
        int result = asm_parsing::parse_and_resolve("jalr x1, 0(x2)", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "jalr");
    }
}

TEST_CASE("Parser - Register aliases", "[parser][aliases]") {
    asm_parsing::ParsedInstVec out;

    SECTION("zero register") {
        int result = asm_parsing::parse_and_resolve("add zero, zero, zero", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
    }

    SECTION("ra register") {
        int result = asm_parsing::parse_and_resolve("add ra, ra, ra", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
    }

    SECTION("sp register") {
        int result = asm_parsing::parse_and_resolve("addi sp, sp, -16", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
    }

    SECTION("gp register") {
        int result = asm_parsing::parse_and_resolve("add gp, gp, x0", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
    }

    SECTION("tp register") {
        int result = asm_parsing::parse_and_resolve("add tp, tp, x0", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
    }

    SECTION("t0-t6 registers") {
        int result = asm_parsing::parse_and_resolve("add t0, t1, t2", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
    }

    SECTION("s0-s11 registers") {
        int result = asm_parsing::parse_and_resolve("add s0, s1, s2", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
    }

    SECTION("a0-a7 registers") {
        int result = asm_parsing::parse_and_resolve("add a0, a1, a2", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
    }

    SECTION("fp alias for s0") {
        int result = asm_parsing::parse_and_resolve("add fp, fp, x0", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
    }
}

TEST_CASE("Parser - Immediate formats", "[parser][immediate]") {
    asm_parsing::ParsedInstVec out;

    SECTION("decimal immediate") {
        int result = asm_parsing::parse_and_resolve("addi x1, x2, 100", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
    }

    SECTION("negative decimal immediate") {
        int result = asm_parsing::parse_and_resolve("addi x1, x2, -100", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
    }

    SECTION("hex immediate lowercase") {
        int result = asm_parsing::parse_and_resolve("addi x1, x2, 0xff", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
    }

    SECTION("hex immediate uppercase") {
        int result = asm_parsing::parse_and_resolve("addi x1, x2, 0xFF", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
    }

    SECTION("zero immediate") {
        int result = asm_parsing::parse_and_resolve("addi x1, x2, 0", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
    }

    SECTION("boundary immediates for I-type") {
        int result = asm_parsing::parse_and_resolve("addi x1, x2, 2047", out, 0);
        REQUIRE(result == 0);
        result = asm_parsing::parse_and_resolve("addi x1, x2, -2048", out, 0);
        REQUIRE(result == 0);
    }
}

TEST_CASE("Parser - Multiple instructions", "[parser][multi]") {
    asm_parsing::ParsedInstVec out;

    SECTION("two instructions") {
        int result = asm_parsing::parse_and_resolve("add x1, x2, x3\nsub x4, x5, x6", out, 0);
        REQUIRE(result == 0);
        REQUIRE(out.size() >= 2);
    }

    SECTION("instructions with labels") {
        int result = asm_parsing::parse_and_resolve(
            "start:\n"
            "  addi x1, x0, 10\n"
            "loop:\n"
            "  addi x1, x1, -1\n"
            "  bne x1, x0, loop\n"
            "end:\n"
            "  add x0, x0, x0",
            out, 0);
        REQUIRE(result == 0);
        REQUIRE(out.size() >= 4);
    }

    SECTION("function prologue/epilogue pattern") {
        int result = asm_parsing::parse_and_resolve(
            "addi sp, sp, -16\n"
            "sd ra, 8(sp)\n"
            "sd s0, 0(sp)\n"
            "addi s0, sp, 16\n"
            "ld s0, 0(sp)\n"
            "ld ra, 8(sp)\n"
            "addi sp, sp, 16\n"
            "jalr x0, 0(ra)",
            out, 0);
        REQUIRE(result == 0);
        REQUIRE(out.size() >= 8);
    }
}

TEST_CASE("Parser - Line numbers", "[parser][lineno]") {
    asm_parsing::ParsedInstVec out;

    SECTION("single instruction has line 1") {
        int result = asm_parsing::parse_and_resolve("add x1, x2, x3", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].lineno == 1);
    }

    SECTION("multiple instructions track line numbers") {
        int result = asm_parsing::parse_and_resolve(
            "add x1, x2, x3\n"
            "sub x4, x5, x6\n"
            "and x7, x8, x9",
            out, 0);
        REQUIRE(result == 0);
        // Check that we have instructions from different lines
        bool has_line_1 = false;
        bool has_line_2 = false;
        bool has_line_3 = false;
        for (const auto &inst : out) {
            if (!inst.is_padding()) {
                if (inst.lineno == 1) has_line_1 = true;
                if (inst.lineno == 2) has_line_2 = true;
                if (inst.lineno == 3) has_line_3 = true;
            }
        }
        REQUIRE(has_line_1);
        REQUIRE(has_line_2);
        REQUIRE(has_line_3);
    }
}

TEST_CASE("Parser - RV64 word operations", "[parser][rv64]") {
    asm_parsing::ParsedInstVec out;

    SECTION("addw instruction") {
        int result = asm_parsing::parse_and_resolve("addw x1, x2, x3", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "addw");
    }

    SECTION("subw instruction") {
        int result = asm_parsing::parse_and_resolve("subw x1, x2, x3", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "subw");
    }

    SECTION("addiw instruction") {
        int result = asm_parsing::parse_and_resolve("addiw x1, x2, 10", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "addiw");
    }

    SECTION("slliw instruction") {
        int result = asm_parsing::parse_and_resolve("slliw x1, x2, 5", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "slliw");
    }

    SECTION("srliw instruction") {
        int result = asm_parsing::parse_and_resolve("srliw x1, x2, 5", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "srliw");
    }

    SECTION("sraiw instruction") {
        int result = asm_parsing::parse_and_resolve("sraiw x1, x2, 5", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "sraiw");
    }
}

TEST_CASE("Parser - M extension", "[parser][rv64m]") {
    asm_parsing::ParsedInstVec out;

    SECTION("mul instruction") {
        int result = asm_parsing::parse_and_resolve("mul x1, x2, x3", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "mul");
    }

    SECTION("div instruction") {
        int result = asm_parsing::parse_and_resolve("div x1, x2, x3", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "div");
    }

    SECTION("rem instruction") {
        int result = asm_parsing::parse_and_resolve("rem x1, x2, x3", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "rem");
    }

    SECTION("mulw instruction") {
        int result = asm_parsing::parse_and_resolve("mulw x1, x2, x3", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "mulw");
    }

    SECTION("divw instruction") {
        int result = asm_parsing::parse_and_resolve("divw x1, x2, x3", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "divw");
    }

    SECTION("remw instruction") {
        int result = asm_parsing::parse_and_resolve("remw x1, x2, x3", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "remw");
    }

    SECTION("mulh instruction") {
        int result = asm_parsing::parse_and_resolve("mulh x1, x2, x3", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "mulh");
    }

    SECTION("mulhu instruction") {
        int result = asm_parsing::parse_and_resolve("mulhu x1, x2, x3", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "mulhu");
    }

    SECTION("mulhsu instruction") {
        int result = asm_parsing::parse_and_resolve("mulhsu x1, x2, x3", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "mulhsu");
    }

    SECTION("divu instruction") {
        int result = asm_parsing::parse_and_resolve("divu x1, x2, x3", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "divu");
    }

    SECTION("remu instruction") {
        int result = asm_parsing::parse_and_resolve("remu x1, x2, x3", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "remu");
    }

    SECTION("divuw instruction") {
        int result = asm_parsing::parse_and_resolve("divuw x1, x2, x3", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "divuw");
    }

    SECTION("remuw instruction") {
        int result = asm_parsing::parse_and_resolve("remuw x1, x2, x3", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "remuw");
    }
}

TEST_CASE("Parser - Shift immediate instructions", "[parser][shift]") {
    asm_parsing::ParsedInstVec out;

    SECTION("slli instruction") {
        int result = asm_parsing::parse_and_resolve("slli x1, x2, 5", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "slli");
    }

    SECTION("srli instruction") {
        int result = asm_parsing::parse_and_resolve("srli x1, x2, 10", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "srli");
    }

    SECTION("srai instruction") {
        int result = asm_parsing::parse_and_resolve("srai x1, x2, 63", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "srai");
    }

    SECTION("slli max shift amount (63)") {
        int result = asm_parsing::parse_and_resolve("slli x1, x2, 63", out, 0);
        REQUIRE(result == 0);
    }

    SECTION("slli zero shift amount") {
        int result = asm_parsing::parse_and_resolve("slli x1, x2, 0", out, 0);
        REQUIRE(result == 0);
    }
}

TEST_CASE("Parser - C extension (compressed instructions)", "[parser][rv64c]") {
    asm_parsing::ParsedInstVec out;

    SECTION("c.nop instruction") {
        int result = asm_parsing::parse_and_resolve("c.nop", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "c.nop");
    }

    SECTION("c.li instruction") {
        int result = asm_parsing::parse_and_resolve("c.li x10, 31", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "c.li");
    }

    SECTION("c.addi instruction") {
        int result = asm_parsing::parse_and_resolve("c.addi x10, 5", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "c.addi");
    }

    SECTION("c.mv instruction") {
        int result = asm_parsing::parse_and_resolve("c.mv x10, x5", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "c.mv");
    }

    SECTION("c.add instruction") {
        int result = asm_parsing::parse_and_resolve("c.add x10, x5", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "c.add");
    }

    SECTION("c.lwsp instruction") {
        int result = asm_parsing::parse_and_resolve("c.lwsp x10, 4", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "c.lwsp");
    }

    SECTION("c.ldsp instruction") {
        int result = asm_parsing::parse_and_resolve("c.ldsp x10, 8", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "c.ldsp");
    }

    SECTION("c.swsp instruction") {
        int result = asm_parsing::parse_and_resolve("c.swsp x10, 4", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "c.swsp");
    }

    SECTION("c.sdsp instruction") {
        int result = asm_parsing::parse_and_resolve("c.sdsp x10, 8", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
        REQUIRE(out[0].inst.get_prototype().mnemonic == "c.sdsp");
    }

    SECTION("c.j instruction with label") {
        int result = asm_parsing::parse_and_resolve("loop:\n  c.j loop", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
    }

    SECTION("c.beqz instruction with label") {
        int result = asm_parsing::parse_and_resolve("target:\n  c.beqz x8, target", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
    }

    SECTION("c.bnez instruction with label") {
        int result = asm_parsing::parse_and_resolve("end:\n  c.bnez x9, end", out, 0);
        REQUIRE(result == 0);
        REQUIRE_FALSE(out.empty());
    }
}
