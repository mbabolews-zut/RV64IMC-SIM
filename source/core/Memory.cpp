#include "Memory.hpp"
#include "common.hpp"

#include <cassert>
#include <cstring>
#include <format>
#include <span>

#include "rv64/AssemblerUnit.hpp"

namespace {
    constexpr size_t MIN_INSTR_SIZE = 2; // Compressed instructions are 2 bytes
    constexpr size_t MAX_STRING_LEN = 4096; // Max string length to prevent infinite loops
}

Memory::Memory(const Layout &layout, std::span<const uint8_t> program_data)
    : m_layout(layout)
      , m_stack_bottom(layout.stack_base)
      , m_heap_start(layout.data_base + program_data.size())
      , m_data_size(program_data.size() + layout.initial_heap_size)
      , m_stack(layout.stack_size, layout.endianness)
      , m_data(PROGRAM_MEM_LIMIT, layout.endianness) {
    assert(m_data_size <= PROGRAM_MEM_LIMIT);

    // Load program data into memory
    for (size_t i = 0; i < program_data.size(); ++i) {
        [[maybe_unused]] bool ok = m_data.store(i, program_data[i]);
        assert(ok);
    }
}

std::string Memory::load_string(uint64_t address, MemErr &err) const {
    std::string result;
    result.reserve(64);

    for (size_t i = 0; i < MAX_STRING_LEN; ++i) {
        uint8_t ch = 0;
        uint64_t byte_addr = address + i;

        // Try data segment first, then stack
        if (in_data(byte_addr, 1)) {
            if (!m_data.load(to_data_offset(byte_addr), ch)) {
                err = MemErr::SegFault;
                return "";
            }
        } else if (in_stack(byte_addr, 1)) {
            if (!m_stack.load(to_stack_offset(byte_addr), ch)) {
                err = MemErr::SegFault;
                return "";
            }
        } else {
            err = MemErr::SegFault;
            return "";
        }

        if (ch == 0) {
            err = MemErr::None;
            return result;
        }
        result.push_back(static_cast<char>(ch));
    }

    err = MemErr::NotTermStr;
    return result;
}

void Memory::load_program(const asm_parsing::ParsedInstVec &instructions) {
    m_instructions = instructions;
    auto bytecode = rv64::AssemblerUnit::assemble(instructions, m_layout.endianness);
    m_data_size += bytecode.size();
    if (m_data_size > PROGRAM_MEM_LIMIT) {
        throw std::runtime_error("Program exceeds memory limit after loading");
    }
    // Load bytecode into data segment
    std::ranges::copy(bytecode, m_data.begin());

    // Update heap start
    m_heap_start = m_layout.data_base + bytecode.size();
}

Memory::InstructionFetch Memory::get_instruction_at(uint64_t address, MemErr &err) const {
    assert(!m_instructions.empty());

    size_t relative_addr = address - m_layout.data_base;
    size_t offset = relative_addr / MIN_INSTR_SIZE;

    // check if end of program has been reached
    if (offset == m_instructions.size()) {
        err = MemErr::ProgramExit;
        return {Instruction::invalid(), std::nullopt};
    }

    // check if address is in instruction memory range
    if (address < m_layout.data_base || offset > m_instructions.size()) {
        err = MemErr::SegFault;
        return {Instruction::invalid(), std::nullopt};
    }

    const auto &parsed_inst = m_instructions.at(offset);

    // check for padding (invalid instruction fetch)
    if (parsed_inst.is_padding()) {
        err = MemErr::InvalidInstructionAddress;
        return {Instruction::invalid(), std::nullopt};
    }

    err = MemErr::None;
    return {parsed_inst.inst, parsed_inst.lineno};
}

uint64_t Memory::get_instruction_end_addr() const {
    return m_layout.data_base + m_instructions.size() * MIN_INSTR_SIZE;
}

std::string Memory::err_to_string(MemErr err) {
    switch (err) {
        case MemErr::None:
            return "No error";
        case MemErr::SegFault:
            return "Segmentation fault";
        case MemErr::NotTermStr:
            return "Segfault: String is not null-terminated";
        case MemErr::OutOfMemory:
            return "Hypervisor could not allocate memory";
        case MemErr::NegativeSizeOfHeap:
            return "Heap size became negative";
        case MemErr::InvalidInstructionAddress:
            return "Invalid instruction address (between instructions)";
        case MemErr::ProgramExit:
            assert(false && "ProgramExit should be handled, not reported as an error");
        default:
            return "Unknown error";
    }
}

uint64_t Memory::sbrk(int64_t inc, MemErr &err) {
    uint64_t old_brk = get_brk();

    if (inc == 0)
        return old_brk;

    // Calculate new data size after increment
    auto heap_offset = m_heap_start - m_layout.data_base;
    int64_t new_size = static_cast<int64_t>(m_data_size) + inc;

    // Ensure heap doesn't shrink below program data
    if (new_size < static_cast<int64_t>(heap_offset)) {
        err = MemErr::NegativeSizeOfHeap;
        return 0;
    }

    // Ensure we don't exceed memory limit
    if (new_size > static_cast<int64_t>(PROGRAM_MEM_LIMIT)) {
        err = MemErr::OutOfMemory;
        return 0;
    }

    m_data_size = static_cast<size_t>(new_size);
    err = MemErr::None;
    return old_brk;
}

uint64_t Memory::get_brk() const {
    return m_layout.data_base + m_data_size;
}

size_t Memory::get_data_size() const {
    return m_data_size;
}

const Memory::Layout &Memory::get_layout() const {
    return m_layout;
}

std::optional<std::string> Memory::validate_layout(const Layout &layout) {
    if (layout.data_base & 1)
        return "Data base address must be even";
    if (layout.data_base + layout.initial_heap_size > layout.data_base + PROGRAM_MEM_LIMIT)
        return "Initial heap size exceeds program memory limit";
    uint64_t data_end = layout.data_base + PROGRAM_MEM_LIMIT;
    uint64_t stack_end = layout.stack_base + layout.stack_size;
    if (layout.data_base < stack_end && data_end > layout.stack_base)
        return std::format("Data and stack memory regions may overlap.\nData region may expand up to {} KiB.",
                           PROGRAM_MEM_LIMIT / 1024);
    return std::nullopt;
}

bool Memory::in_stack(uint64_t address, size_t obj_size) const noexcept {
    return address >= m_stack_bottom && address + obj_size <= stack_end_addr();
}

bool Memory::in_data(uint64_t address, size_t obj_size) const noexcept {
    return address >= m_layout.data_base && address + obj_size <= m_layout.data_base + m_data_size;
}

uint64_t Memory::to_stack_offset(uint64_t address) const noexcept {
    return address - m_stack_bottom;
}

uint64_t Memory::to_data_offset(uint64_t address) const noexcept {
    return address - m_layout.data_base;
}

uint64_t Memory::stack_end_addr() const noexcept {
    return m_stack_bottom + m_layout.stack_size;
}

template<std::integral T>
T Memory::load(uint64_t address, MemErr &err) const {
    err = MemErr::None;
    T value = 0;

    // Check stack first (more commonly accessed)
    if (in_stack(address, sizeof(T))) {
        if (!m_stack.load(to_stack_offset(address), value)) {
            err = MemErr::SegFault;
            return 0;
        }
        return value;
    }

    // Then check data segment
    if (in_data(address, sizeof(T))) {
        if (!m_data.load(to_data_offset(address), value)) {
            err = MemErr::SegFault;
            return 0;
        }
        return value;
    }

    err = MemErr::SegFault;
    return 0;
}

template<std::integral T>
MemErr Memory::store(uint64_t address, T value) {
    // Check stack first (more commonly accessed for writes)
    if (in_stack(address, sizeof(T))) {
        return m_stack.store(to_stack_offset(address), value)
                   ? MemErr::None
                   : MemErr::SegFault;
    }

    // Then check data segment
    if (in_data(address, sizeof(T))) {
        return m_data.store(to_data_offset(address), value)
                   ? MemErr::None
                   : MemErr::SegFault;
    }

    return MemErr::SegFault;
}

// Explicit template instantiations
#define INSTANTIATE_LOAD(TYPE) \
    template TYPE Memory::load(uint64_t address, MemErr &err) const;
#define INSTANTIATE_STORE(TYPE) \
    template MemErr Memory::store(uint64_t address, TYPE value);

FOR_EACH_INT(INSTANTIATE_LOAD)
FOR_EACH_INT(INSTANTIATE_STORE)
