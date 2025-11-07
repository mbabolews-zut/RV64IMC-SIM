#include <format>
#include <iostream>
#include <vector>
#include <string>
#include <rv64/VM.hpp>

int main() {
    rv64::VM vm{};
    asm_parsing::ParsedInstVec inst_vec{};

    // Read standard input
    std::vector<std::string> lines;
    lines.reserve(8);
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

    // Parse and load program
    asm_parsing::parse(inst_vec, whole_input);
    vm.load_program(inst_vec);

    // Helper function to print separator
    auto print_separator = [](bool nl_before = false) {
        std::cout << (nl_before ? "\n\n" : "")
                << "\033[0;32m----------------------------------------------"
                "--------------------------------------------------\033[0m\n";
    };

    // Helper function to print lines with current line indicator
    auto print_lines = [&](size_t current_line) {
        for (size_t i = 0; i < lines.size(); i++) {
            std::cout << (i == current_line ? "\033[0;34m> \033[7m" : "  ")
                    << std::format("{:<92}", lines.at(i)) << "\033[0m\n";
        }
    };

    // Print initial state
    print_separator(true);
    print_lines(0);
    print_separator();
    vm.m_cpu.print_cpu_state();

    // Execute program step by step
    size_t current_line = 1;
    while (vm.get_state() != rv64::VMState::Error &&
           vm.get_state() != rv64::VMState::Finished) {
        print_separator(true);
        print_lines(current_line);
        print_separator();

        vm.run_step();
        vm.m_cpu.print_cpu_state();
        current_line = vm.m_cpu.get_current_line() + 1;
    }

    return 0;
}
