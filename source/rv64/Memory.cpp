#include "Memory.hpp"
#include "common.hpp"

#include <algorithm>
#include <cassert>
#include <format>

#include <rv64/VM.hpp>
#include <stdexcept>
#include <cstring>


Memory::Memory() {
    m_stack.reserve(VEC_INIT_CAPACITY);
    m_data.reserve(VEC_INIT_CAPACITY);
}

std::string Memory::load_string(uint64_t address, MemErr &err) const {
    assert(m_initialized);
    size_t len = 0;
    size_t max_len = 0;
    const char *str_ptr = nullptr;

    if (addr_in_data(address)) {
        auto offset = data_addr_to_offset(address);
        str_ptr = (const char *)(&m_data[offset]) ;
        max_len = m_data.size() - offset;
    } else if (addr_in_stack(address)) {
        auto offset = stack_addr_to_offset(address);
        str_ptr = (const char *)(&m_stack[offset]);
        max_len = m_stack.size() - offset;
    } else {
        err = MemErr::SegFault;
        return "";
    }
    len = strnlen(str_ptr, max_len);
    err = (len == max_len) ? MemErr::NotTermStr : MemErr::None;
    return {str_ptr, len};
}

void Memory::init(std::span<const uint8_t> program_data, const Config &conf) {
    m_config = conf;
    init(program_data);
}

void Memory::init(std::span<const uint8_t> program_data) {
    assert(!m_initialized);
    m_stack_bottom = (m_config.asc_stack
                          ? m_config.stack_addr
                          : m_config.stack_addr - m_config.stack_size);

    m_data.resize(program_data.size() + m_config.initial_heap_size);
    std::ranges::copy(program_data, m_data.data());

    m_heap_addr = m_config.data_addr + program_data.size();

    bool asc = m_config.asc_stack;

    if (!asc && (m_config.stack_addr < m_config.stack_size)) {
        throw std::invalid_argument("Memory: stack address is too small");
    }
    if (asc && (m_config.stack_addr + m_config.stack_size < m_config.stack_addr)) {
        throw std::invalid_argument("Memory: stack address is too large");
    }

    m_stack_bottom = m_config.stack_addr - (asc ? 0 : m_config.stack_size);

    const auto data_start = m_config.data_addr;
    const auto data_end = m_config.data_addr + PROGRAM_MEM_LIMIT;

    bool overlap = asc
                       ? (m_stack_bottom >= data_start && m_stack_bottom < data_end)
                       : (m_config.stack_addr >= data_start && m_config.stack_addr < data_end);

    if (overlap)
        throw std::invalid_argument(std::format(
            "Memory: Stack and program memory may overlap.\n"
            " - Program memory range: [0x{:#X}; 0x{:#X})\n"
            " - Stack range: [0x{:#X}; 0x{:#X})\n"
            " - Stack direction: {}",
            m_config.data_addr,
            m_config.data_addr + PROGRAM_MEM_LIMIT,
            m_stack_bottom,
            stack_end(),
            asc ? "ascending" : "descending"
        ));


    m_initialized = true;
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

uint64_t Memory::sbrk(int64_t inc, MemErr &err) {
    assert(m_initialized);
    uint64_t old_brk = get_brk();

    if (inc == 0)
        return old_brk;

    auto heap_off = m_heap_addr - m_config.data_addr;
    if (inc + m_data.size() < heap_off) {
        err = MemErr::NegativeSizeOfHeap;
        return 0;
    }

    if (inc + m_data.size() > PROGRAM_MEM_LIMIT) {
        err = MemErr::OutOfMemory;
        return 0;
    }

    try {
        m_data.resize(m_data.size() + inc);
    } catch (const std::exception &) {
        err = MemErr::OutOfMemory;
        return 0;
    }
    err = MemErr::None;
    return old_brk;
}

uint64_t Memory::get_brk() const {
    return m_config.data_addr + m_data.size();
}

size_t Memory::get_program_space_size() const {
    return m_data.size();
}

const Memory::Config & Memory::get_conf() const {
    return m_config;
}

bool Memory::addr_in_stack(uint64_t address, size_t obj_size) const noexcept {
    return address >= m_stack_bottom && address + obj_size <= stack_end();
}

bool Memory::addr_in_data(uint64_t address, size_t obj_size) const noexcept {
    return address >= m_config.data_addr && address + obj_size <= m_data.size() + m_config.data_addr;
}

uint64_t Memory::stack_addr_to_offset(uint64_t address) const noexcept {
    return m_config.asc_stack
               ? address - m_stack_bottom
               : stack_end() - address;
}

uint64_t Memory::data_addr_to_offset(uint64_t address) const noexcept {
    return address - m_config.data_addr;
}

template<typename T>
T Memory::load(uint64_t address, MemErr &err) const {
    assert(m_initialized);
    err = MemErr::None;
    if (addr_in_stack(address, sizeof(T))) {
        auto offset = stack_addr_to_offset(address);

        // uninitialized stack memory access
        if (offset + sizeof(T) >= m_stack.size()) {
            if (offset >= m_stack.size())
                return 0;
            std::array<uint8_t, sizeof(T)> buffer{};
            std::ranges::fill(buffer, 0);
            std::copy(&m_stack[offset], m_stack.data() + m_stack.size(), buffer.begin());

            return loadT<T>(buffer);
        }

        return loadT<T>({&m_stack[offset], sizeof(T)});
    }
    if (addr_in_data(address, sizeof(T)))
        return loadT<T>({&m_data[address - m_config.data_addr], sizeof(T)});

    err = MemErr::SegFault;
    return 0;
}

template<typename T>
MemErr Memory::store(uint64_t address, T value) {
    assert(m_initialized);
    if (addr_in_stack(address, sizeof(T))) {
        auto offset = stack_addr_to_offset(address);

        // expand stack if needed
        if (offset + sizeof(T)> m_stack.size()) {
            try {
                m_stack.resize(offset + sizeof(T));
            } catch (const std::exception &) {
                return MemErr::OutOfMemory;
            }
        }
        storeT({&m_stack[offset], sizeof(T)}, value);
        return MemErr::None;
    }
    if (addr_in_data(address, sizeof(T))) {
        storeT({&m_data[address - m_config.data_addr], sizeof(T)}, value);
        return MemErr::None;
    }
    return MemErr::SegFault;
}

#define INSTANTIATE_LOAD(TYPE) \
    template TYPE Memory::load(uint64_t address, MemErr &err) const;
#define INSTANTIATE_STORE(TYPE) \
    template MemErr Memory::store(uint64_t address, TYPE value);

FOR_EACH_INT(INSTANTIATE_LOAD)
FOR_EACH_INT(INSTANTIATE_STORE)
