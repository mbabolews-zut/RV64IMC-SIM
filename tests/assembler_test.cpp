#include <catch2/catch_test_macros.hpp>
#include <parser/asm_parsing.hpp>
#include <rv64/AssemblerUnit.hpp>

#include <array>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <format>
#include <fstream>
#include <memory>
#include <span>
#include <string>
#include <vector>

namespace fs = std::filesystem;

// ============================================================================
// GNU Toolchain Integration Utilities
// ============================================================================

namespace toolchain {

// Check if riscv64-unknown-elf-as is available
inline bool is_available() {
    return std::system("which riscv64-unknown-elf-as > /dev/null 2>&1") == 0;
}

// Execute a command and return its stdout
inline std::string exec(std::string_view cmd) {
    std::unique_ptr<FILE, int(*)(FILE*)> pipe(popen(cmd.data(), "r"), pclose);
    if (!pipe) {
        throw std::runtime_error(std::format("popen() failed for: {}", cmd));
    }
    std::string result;
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe.get()) != nullptr) {
        result += buffer;
    }
    return result;
}

// Execute a command and return exit code
inline int exec_status(std::string_view cmd) {
    return WEXITSTATUS(std::system(cmd.data()));
}

// RAII temp directory
struct TempDir {
    fs::path path;

    TempDir() {
        path = fs::temp_directory_path() / std::format("rv64_test_{}", std::rand());
        fs::create_directories(path);
    }

    ~TempDir() {
        std::error_code ec;
        fs::remove_all(path, ec);
    }

    fs::path operator/(std::string_view name) const {
        return path / name;
    }
};

// Read binary file into vector
inline std::vector<uint8_t> read_binary(const fs::path& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error(std::format("Failed to open: {}", path.string()));
    }
    return std::vector<uint8_t>(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    );
}

// Write string to file
inline void write_file(const fs::path& path, std::string_view content) {
    std::ofstream file(path);
    if (!file) {
        throw std::runtime_error(std::format("Failed to create: {}", path.string()));
    }
    file << content;
}

// Result of toolchain assembly
struct AssemblyResult {
    bool success = false;
    std::vector<uint8_t> bytecode;
    std::string error;
};

// Assemble using GNU toolchain (riscv64-unknown-elf-as)
// Returns raw .text section binary
inline AssemblyResult assemble_gnu(std::string_view source,
                                   std::string_view arch = "rv64imc") {
    AssemblyResult result;

    TempDir tmp;
    auto asm_file = tmp / "test.s";
    auto obj_file = tmp / "test.o";
    auto bin_file = tmp / "test.bin";

    // GNU assembler needs proper directives
    std::string full_source = std::format(
        ".section .text\n"
        ".globl _start\n"
        "_start:\n{}", source);

    write_file(asm_file, full_source);

    // Assemble
    auto as_cmd = std::format("riscv64-unknown-elf-as -march={} -o {} {} 2>&1",
                              arch, obj_file.string(), asm_file.string());

    std::string as_output = exec(as_cmd);
    if (!fs::exists(obj_file)) {
        result.error = std::format("Assembly failed: {}", as_output);
        return result;
    }

    // Extract .text section as raw binary
    auto objcopy_cmd = std::format(
        "riscv64-unknown-elf-objcopy -O binary --only-section=.text {} {} 2>&1",
        obj_file.string(), bin_file.string());

    std::string objcopy_output = exec(objcopy_cmd);
    if (!fs::exists(bin_file)) {
        result.error = std::format("objcopy failed: {}", objcopy_output);
        return result;
    }

    result.bytecode = read_binary(bin_file);
    result.success = true;
    return result;
}

// Disassemble binary using objdump (for debugging)
inline std::string disassemble(std::span<const uint8_t> bytecode) {
    TempDir tmp;
    auto bin_file = tmp / "code.bin";

    std::ofstream file(bin_file, std::ios::binary);
    file.write(reinterpret_cast<const char*>(bytecode.data()), bytecode.size());
    file.close();

    auto cmd = std::format("riscv64-unknown-elf-objdump -D -b binary -m riscv:rv64 {} 2>&1",
                           bin_file.string());
    return exec(cmd);
}

// Format bytecode as hex string for comparison
inline std::string hex_dump(std::span<const uint8_t> data) {
    std::string result;
    for (size_t i = 0; i < data.size(); ++i) {
        if (i > 0 && i % 4 == 0) result += " ";
        if (i > 0 && i % 16 == 0) result += "\n";
        result += std::format("{:02x}", data[i]);
    }
    return result;
}

} // namespace toolchain

// ============================================================================
// Comparison Helper
// ============================================================================

// Assemble with our AssemblerUnit and return bytecode
static std::vector<uint8_t> assemble_ours(std::string_view source) {
    asm_parsing::ParsedInstVec instructions;
    int result = asm_parsing::parse_and_resolve(std::string{source}, instructions, 0);
    if (result != 0) {
        auto breakpoint = true;
    }
    REQUIRE(result == 0);
    return rv64::AssemblerUnit::assemble(instructions, std::endian::little);
}

// Compare our assembly output with GNU toolchain
// Use arch="rv64im" for non-compressed tests to prevent auto-compression
static void compare_assembly(std::string_view source,
                             std::string_view arch = "rv64im",
                             std::string_view test_name = "") {
    INFO(std::format("Test: {}", test_name.empty() ? source : test_name));

    // Get our bytecode
    auto our_bytecode = assemble_ours(source);
    INFO(std::format("Our bytecode ({} bytes):\n{}",
         our_bytecode.size(), toolchain::hex_dump(our_bytecode)));

    // Get GNU toolchain bytecode (use rv64im to disable auto-compression)
    auto gnu_result = toolchain::assemble_gnu(source, arch);
    REQUIRE(gnu_result.success);
    INFO(std::format("GNU bytecode ({} bytes):\n{}",
         gnu_result.bytecode.size(), toolchain::hex_dump(gnu_result.bytecode)));

    // Compare
    REQUIRE(our_bytecode.size() == gnu_result.bytecode.size());

    for (size_t i = 0; i < our_bytecode.size(); ++i) {
        if (our_bytecode[i] != gnu_result.bytecode[i]) {
            INFO(std::format("Mismatch at byte {}: ours=0x{:02x} gnu=0x{:02x}",
                 i, our_bytecode[i], gnu_result.bytecode[i]));

            // Show disassembly for debugging
            if (toolchain::is_available()) {
                INFO(std::format("Our disassembly:\n{}", toolchain::disassemble(our_bytecode)));
                INFO(std::format("GNU disassembly:\n{}", toolchain::disassemble(gnu_result.bytecode)));
            }
        }
        REQUIRE(our_bytecode[i] == gnu_result.bytecode[i]);
    }
}

// ============================================================================
// Tests
// ============================================================================

TEST_CASE("Toolchain available check", "[assembler][toolchain]") {
    if (!toolchain::is_available()) {
        WARN("riscv64-unknown-elf-as not found - skipping toolchain tests");
        WARN("Install with: apt install gcc-riscv64-unknown-elf");
        SKIP("GNU toolchain not available");
    }
    SUCCEED("GNU RISC-V toolchain is available");
}

TEST_CASE("AssemblerUnit vs GNU - R-type instructions", "[assembler][toolchain][rv64i]") {
    if (!toolchain::is_available()) SKIP("GNU toolchain not available");

    SECTION("add") { compare_assembly("add x1, x2, x3"); }
    SECTION("sub") { compare_assembly("sub x5, x6, x7"); }
    SECTION("and") { compare_assembly("and x10, x11, x12"); }
    SECTION("or")  { compare_assembly("or x13, x14, x15"); }
    SECTION("xor") { compare_assembly("xor x16, x17, x18"); }
    SECTION("sll") { compare_assembly("sll x19, x20, x21"); }
    SECTION("srl") { compare_assembly("srl x22, x23, x24"); }
    SECTION("sra") { compare_assembly("sra x25, x26, x27"); }
    SECTION("slt") { compare_assembly("slt x28, x29, x30"); }
    SECTION("sltu") { compare_assembly("sltu x1, x2, x3"); }
}

TEST_CASE("AssemblerUnit vs GNU - I-type instructions", "[assembler][toolchain][rv64i]") {
    if (!toolchain::is_available()) SKIP("GNU toolchain not available");

    SECTION("addi") { compare_assembly("addi x1, x2, 100"); }
    SECTION("addi negative") { compare_assembly("addi x1, x2, -100"); }
    SECTION("andi") { compare_assembly("andi x3, x4, 0xFF"); }
    SECTION("ori")  { compare_assembly("ori x5, x6, 0x123"); }
    SECTION("xori") { compare_assembly("xori x7, x8, -1"); }
    SECTION("slti") { compare_assembly("slti x9, x10, 50"); }
    SECTION("sltiu") { compare_assembly("sltiu x11, x12, 100"); }
    SECTION("slli") { compare_assembly("slli x13, x14, 5"); }
    SECTION("srli") { compare_assembly("srli x15, x16, 10"); }
    SECTION("srai") { compare_assembly("srai x17, x18, 3"); }
}

TEST_CASE("AssemblerUnit vs GNU - Load instructions", "[assembler][toolchain][rv64i]") {
    if (!toolchain::is_available()) SKIP("GNU toolchain not available");

    SECTION("lb")  { compare_assembly("lb x1, 0(x2)"); }
    SECTION("lh")  { compare_assembly("lh x3, 4(x4)"); }
    SECTION("lw")  { compare_assembly("lw x5, 8(x6)"); }
    SECTION("ld")  { compare_assembly("ld x7, 16(x8)"); }
    SECTION("lbu") { compare_assembly("lbu x9, -4(x10)"); }
    SECTION("lhu") { compare_assembly("lhu x11, -8(x12)"); }
    SECTION("lwu") { compare_assembly("lwu x13, 100(x14)"); }
}

TEST_CASE("AssemblerUnit vs GNU - Store instructions", "[assembler][toolchain][rv64i]") {
    if (!toolchain::is_available()) SKIP("GNU toolchain not available");

    SECTION("sb") { compare_assembly("sb x1, 0(x2)"); }
    SECTION("sh") { compare_assembly("sh x3, 4(x4)"); }
    SECTION("sw") { compare_assembly("sw x5, 8(x6)"); }
    SECTION("sd") { compare_assembly("sd x7, -16(x8)"); }
}

TEST_CASE("AssemblerUnit vs GNU - U-type instructions", "[assembler][toolchain][rv64i]") {
    if (!toolchain::is_available()) SKIP("GNU toolchain not available");

    SECTION("lui") { compare_assembly("lui x1, 0x12345"); }
    SECTION("lui max") { compare_assembly("lui x2, 0xFFFFF"); }
    SECTION("auipc") { compare_assembly("auipc x3, 0x1000"); }
}

TEST_CASE("AssemblerUnit vs GNU - Branch instructions", "[assembler][toolchain][rv64i]") {
    if (!toolchain::is_available()) SKIP("GNU toolchain not available");

    SECTION("beq forward") {
        compare_assembly(R"(
            beq x1, x2, target
            addi x3, x0, 1
        target:
            addi x4, x0, 2
        )");
    }

    SECTION("bne forward") {
        compare_assembly(R"(
            bne x1, x2, target
            addi x3, x0, 1
        target:
            addi x4, x0, 2
        )");
    }

    SECTION("blt forward") {
        compare_assembly(R"(
            blt x1, x2, target
            addi x3, x0, 1
        target:
            addi x4, x0, 2
        )");
    }

    SECTION("bge forward") {
        compare_assembly(R"(
            bge x1, x2, target
            addi x3, x0, 1
        target:
            addi x4, x0, 2
        )");
    }

    SECTION("bltu forward") {
        compare_assembly(R"(
            bltu x1, x2, target
            addi x3, x0, 1
        target:
            addi x4, x0, 2
        )");
    }

    SECTION("bgeu forward") {
        compare_assembly(R"(
            bgeu x1, x2, target
            addi x3, x0, 1
        target:
            addi x4, x0, 2
        )");
    }
}

TEST_CASE("AssemblerUnit vs GNU - Jump instructions", "[assembler][toolchain][rv64i]") {
    if (!toolchain::is_available()) SKIP("GNU toolchain not available");

    SECTION("jal forward") {
        compare_assembly(R"(
            jal x1, target
            addi x2, x0, 1
        target:
            addi x3, x0, 2
        )");
    }

    SECTION("jalr") { compare_assembly("jalr x1, x2, 0"); }
    SECTION("jalr offset") { compare_assembly("jalr x1, x2, 100"); }
}

TEST_CASE("AssemblerUnit vs GNU - RV64I W-suffix instructions", "[assembler][toolchain][rv64i]") {
    if (!toolchain::is_available()) SKIP("GNU toolchain not available");

    SECTION("addw") { compare_assembly("addw x1, x2, x3"); }
    SECTION("subw") { compare_assembly("subw x4, x5, x6"); }
    SECTION("sllw") { compare_assembly("sllw x7, x8, x9"); }
    SECTION("srlw") { compare_assembly("srlw x10, x11, x12"); }
    SECTION("sraw") { compare_assembly("sraw x13, x14, x15"); }
    SECTION("addiw") { compare_assembly("addiw x16, x17, 50"); }
    SECTION("slliw") { compare_assembly("slliw x18, x19, 5"); }
    SECTION("srliw") { compare_assembly("srliw x20, x21, 10"); }
    SECTION("sraiw") { compare_assembly("sraiw x22, x23, 3"); }
}

TEST_CASE("AssemblerUnit vs GNU - M extension", "[assembler][toolchain][rv64m]") {
    if (!toolchain::is_available()) SKIP("GNU toolchain not available");

    SECTION("mul") { compare_assembly("mul x1, x2, x3"); }
    SECTION("mulh") { compare_assembly("mulh x4, x5, x6"); }
    SECTION("mulhsu") { compare_assembly("mulhsu x7, x8, x9"); }
    SECTION("mulhu") { compare_assembly("mulhu x10, x11, x12"); }
    SECTION("div") { compare_assembly("div x13, x14, x15"); }
    SECTION("divu") { compare_assembly("divu x16, x17, x18"); }
    SECTION("rem") { compare_assembly("rem x19, x20, x21"); }
    SECTION("remu") { compare_assembly("remu x22, x23, x24"); }
    SECTION("mulw") { compare_assembly("mulw x25, x26, x27"); }
    SECTION("divw") { compare_assembly("divw x28, x29, x30"); }
    SECTION("divuw") { compare_assembly("divuw x1, x2, x3"); }
    SECTION("remw") { compare_assembly("remw x4, x5, x6"); }
    SECTION("remuw") { compare_assembly("remuw x7, x8, x9"); }
}

TEST_CASE("AssemblerUnit vs GNU - C extension (compressed)", "[assembler][toolchain][rv64c]") {
    if (!toolchain::is_available()) SKIP("GNU toolchain not available");

    constexpr auto C = "rv64imc";  // Enable C extension for these tests

    SECTION("c.addi") { compare_assembly("c.addi x10, 5", C); }
    SECTION("c.li") { compare_assembly("c.li x11, 10", C); }
    SECTION("c.lui") { compare_assembly("c.lui x12, 1", C); }
    SECTION("c.mv") { compare_assembly("c.mv x13, x14", C); }
    SECTION("c.add") { compare_assembly("c.add x15, x16", C); }
    SECTION("c.nop") { compare_assembly("c.nop", C); }

    // Stack-relative loads/stores (GNU requires offset(sp) format)
    SECTION("c.lwsp") { compare_assembly("c.lwsp x10, 0(sp)", C); }
    SECTION("c.ldsp") { compare_assembly("c.ldsp x11, 8(sp)", C); }
    SECTION("c.swsp") { compare_assembly("c.swsp x12, 0(sp)", C); }
    SECTION("c.sdsp") { compare_assembly("c.sdsp x13, 8(sp)", C); }

    // Register-register operations (x8-x15 only)
    SECTION("c.sub") { compare_assembly("c.sub x8, x9", C); }
    SECTION("c.xor") { compare_assembly("c.xor x10, x11", C); }
    SECTION("c.or") { compare_assembly("c.or x12, x13", C); }
    SECTION("c.and") { compare_assembly("c.and x14, x15", C); }
    SECTION("c.subw") { compare_assembly("c.subw x8, x9", C); }
    SECTION("c.addw") { compare_assembly("c.addw x10, x11", C); }

    // Shifts
    SECTION("c.slli") { compare_assembly("c.slli x10, 5", C); }
    SECTION("c.srli") { compare_assembly("c.srli x8, 3", C); }
    SECTION("c.srai") { compare_assembly("c.srai x9, 4", C); }

    // Immediate arithmetic (x8-x15 only)
    SECTION("c.addiw") { compare_assembly("c.addiw x10, 10", C); }
    SECTION("c.andi") { compare_assembly("c.andi x8, 15", C); }
}

TEST_CASE("AssemblerUnit vs GNU - Complex programs", "[assembler][toolchain][integration]") {
    if (!toolchain::is_available()) SKIP("GNU toolchain not available");

    SECTION("Fibonacci sequence setup") {
        compare_assembly(R"(
            addi x1, x0, 0
            addi x2, x0, 1
            addi x3, x0, 10
            add x4, x1, x2
            addi x1, x2, 0
            addi x2, x4, 0
            addi x3, x3, -1
        )");
    }

    SECTION("Memory operations sequence") {
        compare_assembly(R"(
            lui x1, 0x400
            addi x2, x0, 42
            sw x2, 0(x1)
            lw x3, 0(x1)
            addi x3, x3, 8
            sw x3, 4(x1)
        )");
    }

    SECTION("Branch and jump mix") {
        compare_assembly(R"(
            addi x1, x0, 5
            addi x2, x0, 5
            beq x1, x2, equal
            addi x3, x0, 0
            jal x0, done
        equal:
            addi x3, x0, 1
        done:
            addi x4, x0, 99
        )");
    }

    SECTION("Loop with backward branch") {
        compare_assembly(R"(
            addi x1, x0, 5
            addi x2, x0, 0
        loop:
            addi x2, x2, 1
            addi x1, x1, -1
            bne x1, x0, loop
            addi x3, x0, 100
        )");
    }
}

TEST_CASE("AssemblerUnit vs GNU - Edge cases", "[assembler][toolchain][edge]") {
    if (!toolchain::is_available()) SKIP("GNU toolchain not available");

    SECTION("Maximum immediate values") {
        compare_assembly("addi x1, x0, 2047");
        compare_assembly("addi x1, x0, -2048");
    }

    SECTION("All registers") {
        compare_assembly(R"(
            add x0, x1, x2
            add x3, x4, x5
            add x6, x7, x8
            add x9, x10, x11
            add x12, x13, x14
            add x15, x16, x17
            add x18, x19, x20
            add x21, x22, x23
            add x24, x25, x26
            add x27, x28, x29
            add x30, x31, x0
        )");
    }

    SECTION("ABI register names") {
        compare_assembly(R"(
            add zero, ra, sp
            add gp, tp, t0
            add t1, t2, s0
            add s1, a0, a1
            add a2, a3, a4
            add a5, a6, a7
            add s2, s3, s4
            add s5, s6, s7
            add s8, s9, s10
            add s11, t3, t4
            add t5, t6, zero
        )");
    }
}

// ============================================================================
// Byte-level encoding tests (without toolchain)
// ============================================================================

TEST_CASE("AssemblerUnit encoding sanity", "[assembler][encoding]") {
    SECTION("NOP is encoded correctly") {
        auto bytecode = assemble_ours("addi x0, x0, 0");
        // NOP = addi x0, x0, 0 = 0x00000013
        REQUIRE(bytecode.size() == 4);
        REQUIRE(bytecode[0] == 0x13);
        REQUIRE(bytecode[1] == 0x00);
        REQUIRE(bytecode[2] == 0x00);
        REQUIRE(bytecode[3] == 0x00);
    }

    SECTION("Instruction size") {
        // 32-bit instruction
        auto i32 = assemble_ours("add x1, x2, x3");
        REQUIRE(i32.size() == 4);

        // 16-bit compressed instruction
        auto i16 = assemble_ours("c.nop");
        REQUIRE(i16.size() == 2);
    }

    SECTION("Multiple instructions") {
        auto bytecode = assemble_ours(R"(
            addi x1, x0, 1
            addi x2, x0, 2
            add x3, x1, x2
        )");
        REQUIRE(bytecode.size() == 12); // 3 * 4 bytes
    }
}