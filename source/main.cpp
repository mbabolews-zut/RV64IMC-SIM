#include <iostream>
#include <rv64/VM.hpp>

int main() {
    rv64::VM vm{};
    ParserProcessor parserproc{};
    vm.m_cpu.print_cpu_state();
    asm_parse(parserproc, "ADDI x1, x0, 1\n");
    vm.load_program(parserproc.get_parsed_instructions());
    vm.run_step();
    vm.m_cpu.print_cpu_state();
    return 0;
}