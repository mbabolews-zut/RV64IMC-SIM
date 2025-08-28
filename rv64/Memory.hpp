#pragma once
#include <cstdint>
#include <string>
#include <vector>

enum class MemErr {
    None = 0, SegFault = 1, NotTermStr = 2, OutOfMemory = 3, NegativeSizeOfHeap = 4
};

class Memory {
public:
    static constexpr
    size_t VEC_INIT_CAPACITY = 4096;

    static constexpr
    size_t PROGRAM_MEM_LIMIT = 4294967296; // 4 GiB

    template<typename T>
    T load(uint64_t address, MemErr &err) const;

    template<typename T>
    [[nodiscard]] MemErr store(uint64_t address, T value);

    std::string load_string(uint64_t address, MemErr &err);

    Memory(const void *vm_settings, size_t static_data_size);
    static std::string err_to_string(MemErr err);

    uint64_t sbrk(int64_t increment, MemErr &err);

private:
    std::vector<uint8_t> m_stack{};
    std::vector<uint8_t> m_data{};

    uint64_t m_program_addr;
    uint64_t m_heap_address;
    uint64_t m_stack_address;

    size_t m_stack_size;

    const void *m_vm_settings;
};

template<typename T>
T Memory::load(uint64_t address, MemErr &err) const {
    if (address >= m_stack_address && address + sizeof(T) < m_stack_address + m_stack_size) {
        return *reinterpret_cast<const T*>(m_stack.data() + (address - m_stack_address));
    }
    if (address >= m_program_addr && address + sizeof(T) < m_data.size() + m_program_addr) {
        return *reinterpret_cast<const T*>(m_data.data() + (address - m_program_addr));
    }
    err = MemErr::SegFault;
    return 0;
}

template uint8_t Memory::load<uint8_t>(uint64_t address, MemErr &err) const;
template uint16_t Memory::load<uint16_t>(uint64_t address, MemErr &err) const;
template uint32_t Memory::load<uint32_t>(uint64_t address, MemErr &err) const;
template uint64_t Memory::load<uint64_t>(uint64_t address, MemErr &err) const;
template int8_t Memory::load<int8_t>(uint64_t address, MemErr &err) const;
template int16_t Memory::load<int16_t>(uint64_t address, MemErr &err) const;
template int32_t Memory::load<int32_t>(uint64_t address, MemErr &err) const;
template int64_t Memory::load<int64_t>(uint64_t address, MemErr &err) const;
