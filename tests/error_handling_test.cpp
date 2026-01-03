#include <catch2/catch_test_macros.hpp>
#include <parser/asm_parsing.hpp>
#include <rv64/VM.hpp>
#include <ui.hpp>

// Initialize instruction table once for all tests
[[maybe_unused]] static rv64::VM vm;
std::string err_msg;

TEST_CASE("Error handling - Unknown instruction", "[errors]") {
    ui::set_error_msg_callback([](auto msg) {err_msg = msg;});
    asm_parsing::ParsedInstVec out;
    int result = asm_parsing::parse_and_resolve("addd x1, x2, x3", out, 0x400000);
    REQUIRE(result != 0);
    REQUIRE(out.empty());
    REQUIRE(err_msg.find("Unknown instruction 'addd'") != std::string::npos);
}

TEST_CASE("Error handling - Invalid register", "[errors]") {
    ui::set_error_msg_callback([](auto msg) {err_msg = msg;});
    asm_parsing::ParsedInstVec out;
    int result = asm_parsing::parse_and_resolve("add x1, x32, x3", out, 0x400000);
    REQUIRE(result != 0);
    REQUIRE(out.empty());
    REQUIRE(err_msg.find("Invalid register 'x32'") != std::string::npos);
}

TEST_CASE("Error handling - Immediate out of range", "[errors]") {
    ui::set_error_msg_callback([](auto msg) {err_msg = msg;});
    asm_parsing::ParsedInstVec out;
    int result = asm_parsing::parse_and_resolve("addi x1, x2, 5000", out, 0x400000);
    REQUIRE(result != 0);
    REQUIRE(out.empty());
    REQUIRE(err_msg.find("Immediate 5000 out of range") != std::string::npos);
}

TEST_CASE("Error handling - Duplicate label", "[errors]") {
    ui::set_error_msg_callback([](auto msg) {err_msg = msg;});
    asm_parsing::ParsedInstVec out;
    int result = asm_parsing::parse_and_resolve("loop:\n  add x1, x2, x3\nloop:\n  add x4, x5, x6", out, 0x400000);
    // Note: duplicate label is reported but parsing may still succeed partially
    // The second 'loop' label error is printed but doesn't stop parsing
    REQUIRE(result == 0); // Parsing succeeds, but error is printed
    REQUIRE(err_msg.find("Duplicate label 'loop'") != std::string::npos);
}

TEST_CASE("Error handling - Valid instruction parses correctly", "[errors]") {
    ui::set_error_msg_callback([](auto msg) {err_msg = msg;});
    asm_parsing::ParsedInstVec out;
    int result = asm_parsing::parse_and_resolve("add x1, x2, x3", out, 0x400000);
    REQUIRE(result == 0);
    REQUIRE_FALSE(out.empty());
}

TEST_CASE("Error handling - Valid immediate in range", "[errors]") {
    ui::set_error_msg_callback([](auto msg) {err_msg = msg;});
    asm_parsing::ParsedInstVec out;
    int result = asm_parsing::parse_and_resolve("addi x1, x2, 2047", out, 0x400000);
    REQUIRE(result == 0);
    REQUIRE_FALSE(out.empty());
}

TEST_CASE("Error handling - Negative immediate in range", "[errors]") {
    ui::set_error_msg_callback([](auto msg) {err_msg = msg;});
    asm_parsing::ParsedInstVec out;
    int result = asm_parsing::parse_and_resolve("addi x1, x2, -2048", out, 0x400000);
    REQUIRE(result == 0);
    REQUIRE_FALSE(out.empty());
}

TEST_CASE("Error handling - Immediate at boundary fails", "[errors]") {
    ui::set_error_msg_callback([](auto msg) {err_msg = msg;});
    asm_parsing::ParsedInstVec out;
    int result = asm_parsing::parse_and_resolve("addi x1, x2, 2048", out, 0x400000);
    REQUIRE(result != 0);
    REQUIRE(err_msg.find("Immediate 2048 out of range") != std::string::npos);
}

TEST_CASE("Error handling - Unresolved symbol", "[errors]") {
    ui::set_error_msg_callback([](auto msg) {err_msg = msg;});
    asm_parsing::ParsedInstVec out;
    int result = asm_parsing::parse_and_resolve("beq x1, x2, undefined_label", out, 0x400000);
    REQUIRE(result != 0);
    REQUIRE(err_msg.find("Unresolved symbol 'undefined_label'") != std::string::npos);
}

TEST_CASE("Error handling - Multiple instructions parse", "[errors]") {
    ui::set_error_msg_callback([](auto msg) {err_msg = msg;});
    asm_parsing::ParsedInstVec out;
    int result = asm_parsing::parse_and_resolve("add x1, x2, x3\nsub x4, x5, x6", out, 0);
    REQUIRE(result == 0);
    REQUIRE(out.size() >= 2);  // At least 2 instructions (may have padding)
}
