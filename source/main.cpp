#include <iostream>
#include <vector>
#include <string>
#include <rv64/VM.hpp>

int main() {
    rv64::VM vm{};
    ParserProcessor parser_proc{};

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


    // Parse and load program
    asm_parse(parser_proc, whole_input);
    vm.load_program(parser_proc.get_parsed_instructions());

    // Helper function to print separator
    auto print_separator = [](bool nl_before = false) {
        std::cout << (nl_before ? "\n\n" : "")
                << "----------------------------------------------"
                "--------------------------------------------------\n";
    };

    // Helper function to print lines with current line indicator
    auto print_lines = [&](int current_line) {
        for (size_t i = 0; i < lines.size(); i++) {
            std::cout << (static_cast<int>(i) == current_line ? "> " : "  ")
                    << lines[i] << '\n';
        }
    };

    // Print initial state
    print_separator(true);
    print_lines(0);
    print_separator();
    vm.m_cpu.print_cpu_state();

    // Execute program step by step
    int current_line = 1;
    while (vm.get_state() != rv64::VMState::Error &&
           vm.get_state() != rv64::VMState::Finished) {
        print_separator(true);
        print_lines(current_line);
        print_separator();

        vm.run_step();
        vm.m_cpu.print_cpu_state();
        current_line = vm.m_cpu.get_pc() / 4 + 1;
    }

    return 0;
}
