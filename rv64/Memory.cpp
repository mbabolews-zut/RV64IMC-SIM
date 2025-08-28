#include "Memory.hpp"
#include <rv64/VM.hpp>
#include <stdexcept>

// helper to cast vm_settings
static const auto &vmsett_c = [](const void *vm_settings) {
    return *(rv64::VM::Settings *) vm_settings;
};

Memory::Memory(const void *vm_settings, size_t static_data_size)
    : m_vm_settings(vm_settings),
      m_stack_address(vmsett_c(vm_settings).stack_start_address),
      m_stack_size(vmsett_c(vm_settings).stack_size),
      m_program_addr(vmsett_c(vm_settings).program_start_address),
      m_heap_address(m_program_addr + static_data_size) {
    // reserve some space in vectors
    m_stack.reserve(VEC_INIT_CAPACITY);
    m_data.reserve(VEC_INIT_CAPACITY);

    if (m_stack_address < m_stack_size) {
        throw std::invalid_argument("Memory: stack address is too small");
    }

    auto stack_bottom = m_stack_address - m_stack_size;

    if (stack_bottom >= m_program_addr && stack_bottom < m_program_addr + PROGRAM_MEM_LIMIT)
        throw std::invalid_argument("Memory: stack and program memory overlap");
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
        default:
            return "Unknown error";
    }
}
