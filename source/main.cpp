#include <format>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>
#include <rv64/VM.hpp>

int main() {
    rv64::VM vm{};

    std::vector<std::string> lines;
    std::string whole_input;
    std::string line;

    while (std::getline(std::cin, line)) {
        whole_input.append(line + '\n');
        lines.push_back(line);
    }

    if (lines.empty()) {
        return 0;
    }
    lines.emplace_back(" ");

    // Parse and resolve in one step
    asm_parsing::ParsedInstVec inst_vec;
    int result = asm_parsing::parse_and_resolve(whole_input, inst_vec, vm.m_cpu.get_pc());
    if (result != 0) {
        std::cerr << "Error: Failed to process assembly code (error code " << result << ")\n";
        return 1;
    }
    vm.load_program(inst_vec);

    auto print_separator = [](bool nl_before = false) {
        std::cout << (nl_before ? "\n\n" : "")
                << "\033[0;32m----------------------------------------------"
                "--------------------------------------------------\033[0m\n";
    };

    auto print_lines = [&](int64_t current_lineno) {
        for (size_t i = 0; i < lines.size(); i++) {
            bool is_current = (i == current_lineno - 1);
            std::cout << (is_current ? "\033[0;34m> \033[7m" : "  ")
                    << std::format("{:<92}", lines.at(i)) << "\033[0m\n";
        }
    };

    print_separator(true);
    print_lines(1);
    print_separator();
    vm.m_cpu.print_cpu_state();

    auto current_lineno = (int64_t)vm.get_current_line();
    while (vm.get_state() != rv64::VMState::Error &&
           vm.get_state() != rv64::VMState::Finished) {
        vm.run_step();
        current_lineno = vm.get_current_line();
        print_separator(true);
        print_lines(current_lineno);
        print_separator();
        vm.m_cpu.print_cpu_state();
    }

    return 0;
}
