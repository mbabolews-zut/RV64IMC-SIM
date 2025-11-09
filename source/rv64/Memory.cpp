#include "Memory.hpp"
#include "common.hpp"

#include <cassert>
#include <format>

namespace {
    constexpr size_t MIN_INSTR_SIZE = 2; // Compressed instructions are 2 bytes
    constexpr size_t MAX_STRING_LEN = 4096; // Max string length to prevent infinite loops
}

Memory::Memory(const Layout &layout, std::span<const uint8_t> program_data)
    : m_layout(layout)
    , m_stack_bottom(layout.stack_base)
    , m_heap_start(layout.data_base + program_data.size())
    , m_data_size(program_data.size() + layout.initial_heap_size)
{
    assert(m_data_size <= PROGRAM_MEM_LIMIT);

    // Allocate paged memory for stack and data segments
    m_stack = std::make_unique<PagedMemory>(layout.stack_size, layout.endianness);
    m_data = std::make_unique<PagedMemory>(PROGRAM_MEM_LIMIT, layout.endianness);

    // Load program data into memory
    for (size_t i = 0; i < program_data.size(); ++i) {
        [[maybe_unused]] bool ok = m_data->store(i, program_data[i]);
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
            if (!m_data->load(to_data_offset(byte_addr), ch)) {
                err = MemErr::SegFault;
                return "";
            }
        } else if (in_stack(byte_addr, 1)) {
            if (!m_stack->load(to_stack_offset(byte_addr), ch)) {
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
}

const Instruction &Memory::get_instruction_at(uint64_t address, MemErr &err, size_t *line) const {
    assert(!m_instructions.empty());
    if (line) *line = SIZE_MAX;

    // Calculate instruction index based on minimum instruction size
    size_t offset = (address - m_layout.data_base) / MIN_INSTR_SIZE;

    // Check if we've reached the end of the program
    if (offset == m_instructions.size()) {
        err = MemErr::ProgramExit;
        return Instruction::get_invalid_cref();
    }

    // Validate address is within instruction memory range
    if (address < m_layout.data_base || offset > m_instructions.size()) {
        err = MemErr::SegFault;
        return Instruction::get_invalid_cref();
    }

    const auto &parsed_inst = m_instructions.at(offset);
    err = MemErr::None;
    if (line) *line = parsed_inst.lineno;
    return parsed_inst.inst;
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
        case MemErr::UnalignedAccess:
            return "Unaligned memory access";
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

    // Check alignment for multi-byte values (RISC-V requirement)
    if constexpr (sizeof(T) > 1) {
        if (address % sizeof(T) != 0) {
            err = MemErr::UnalignedAccess;
            return 0;
        }
    }

    // Check stack first (more commonly accessed)
    if (in_stack(address, sizeof(T))) {
        if (!m_stack->load(to_stack_offset(address), value)) {
            err = MemErr::SegFault;
            return 0;
        }
        return value;
    }

    // Then check data segment
    if (in_data(address, sizeof(T))) {
        if (!m_data->load(to_data_offset(address), value)) {
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
    // Check alignment for multi-byte values (RISC-V requirement)
    if constexpr (sizeof(T) > 1) {
        if (address % sizeof(T) != 0) {
            return MemErr::UnalignedAccess;
        }
    }

    // Check stack first (more commonly accessed for writes)
    if (in_stack(address, sizeof(T))) {
        return m_stack->store(to_stack_offset(address), value)
                   ? MemErr::None
                   : MemErr::SegFault;
    }

    // Then check data segment
    if (in_data(address, sizeof(T))) {
        return m_data->store(to_data_offset(address), value)
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


