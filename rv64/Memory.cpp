#include "Memory.hpp"

#include <algorithm>

#include <rv64/VM.hpp>
#include <stdexcept>
#include <cstring>

// helper to cast vm_settings
static const auto &vmsett_c = [](const void *vm_settings) {
    return *(rv64::VM::Settings *) vm_settings;
};

std::string Memory::load_string(uint64_t address, MemErr &err) const {
    size_t len = 0;
    size_t max_len = 0;
    const char *str_ptr = nullptr;

    if (address_in_data(address)) {
        str_ptr = reinterpret_cast<const char *>(m_data.data() + (address - m_program_addr));
        max_len = m_data.size() - (address - m_program_addr);
    } else if (address_in_stack(address)) {
        str_ptr = reinterpret_cast<const char *>(m_stack.data() + (address - m_stack_addr));
        max_len = m_stack_size - (address - m_stack_addr);
    } else {
        err = MemErr::SegFault;
        return "";
    }
    len = strnlen(str_ptr, max_len);
    err = (len == max_len) ? MemErr::NotTermStr : MemErr::None;
    return {str_ptr, len};
}

Memory::Memory(const void *vm_settings, size_t static_data_size)
    : m_program_addr(vmsett_c(vm_settings).program_start_address),
      m_heap_addr(m_program_addr + static_data_size),
      m_stack_addr(vmsett_c(vm_settings).stack_start_address),
      m_stack_size(vmsett_c(vm_settings).stack_size) {
    // reserve some space in vectors
    m_stack.reserve(VEC_INIT_CAPACITY);
    m_data.reserve(VEC_INIT_CAPACITY);

    if (m_stack_addr < m_stack_size) {
        throw std::invalid_argument("Memory: stack address is too small");
    }

    auto stack_bottom = m_stack_addr - m_stack_size;

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

uint64_t Memory::sbrk(int64_t increment, MemErr &err) {
    if (increment > m_data.size() - (m_heap_addr - m_program_addr)) {
        err = MemErr::NegativeSizeOfHeap;
        return 0;
    }
    try {
        m_data.resize(m_data.size() + increment);
    } catch (const std::exception &) {
        err = MemErr::OutOfMemory;
        return 0;
    }
    err = MemErr::None;
    return m_program_addr + m_data.size() - increment;
}

bool Memory::address_in_stack(uint64_t address) const noexcept {
    return address >= m_stack_addr && address < m_stack_addr + m_stack_size;
}

bool Memory::address_in_data(uint64_t address) const noexcept {
    return address >= m_program_addr && address < m_data.size() + m_program_addr;
}

template<typename T>
T Memory::load(uint64_t address, MemErr &err) const {
    if (address_in_stack(address + sizeof(T))) {
        // uninitialized stack memory access
        if (address + sizeof(T) - m_stack_addr > m_stack.size()) {
            T value = 0;
            if (address - m_stack_addr < m_stack.size()) {
                size_t available = m_stack.size() - (address - m_stack_addr);
                std::copy_n(m_stack.data() + (address - m_stack_addr), available, &value);
            }
            return value;
        }

        return *reinterpret_cast<const T *>(m_stack.data() + (address - m_stack_addr));
    }
    if (address_in_data(address + sizeof(T)))
        return *reinterpret_cast<const T *>(m_data.data() + (address - m_program_addr));

    err = MemErr::SegFault;
    return 0;
}

template<typename T>
MemErr Memory::store(uint64_t address, T value) {
    if (address_in_stack(address + sizeof(T))) {
        if (address + sizeof(T) - m_stack_addr > m_stack.size()) {
            try {
                m_stack.resize(address + sizeof(T) - m_stack_addr);
            } catch (const std::exception &) {
                return MemErr::OutOfMemory;
            }
        }
        *reinterpret_cast<T *>(m_stack.data() + (address - m_stack_addr)) = value;
        return MemErr::None;
    }
    if (address_in_data(address + sizeof(T))) {
        *reinterpret_cast<T *>(m_data.data() + (address - m_program_addr)) = value;
        return MemErr::None;
    }
    return MemErr::SegFault;
}

template uint8_t Memory::load(uint64_t, MemErr &) const;
template uint16_t Memory::load(uint64_t, MemErr &) const;
template uint32_t Memory::load(uint64_t, MemErr &) const;
template uint64_t Memory::load(uint64_t, MemErr &) const;
template int8_t Memory::load(uint64_t, MemErr &) const;
template int16_t Memory::load(uint64_t, MemErr &) const;
template int32_t Memory::load(uint64_t, MemErr &) const;
template int64_t Memory::load(uint64_t, MemErr &) const;

template MemErr Memory::store(uint64_t, uint8_t);
template MemErr Memory::store(uint64_t, uint16_t);
template MemErr Memory::store(uint64_t, uint32_t);
template MemErr Memory::store(uint64_t, uint64_t);
template MemErr Memory::store(uint64_t, int8_t);
template MemErr Memory::store(uint64_t, int16_t);
template MemErr Memory::store(uint64_t, int32_t);
template MemErr Memory::store(uint64_t, int64_t);



