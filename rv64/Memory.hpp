#pragma once
#include <cstdint>
#include <string>
#include <vector>

enum class MemErr {
    None = 0, SegFault = 1, NotTermStr = 2, OutOfMemory = 3, NegativeSizeOfHeap = 4
};

class Memory {
public:
    Memory();

    static constexpr
    size_t VEC_INIT_CAPACITY = 4096;

    static constexpr
    size_t PROGRAM_MEM_LIMIT = 1024 * 1024 * 8; // 8 MiB

    template<typename T>
    T load(uint64_t address, MemErr &err) const;

    template<typename T>
    [[nodiscard]] MemErr store(uint64_t address, T value);

    std::string load_string(uint64_t address, MemErr &err) const;


    template<typename VMSettingsT>
    void init(const VMSettingsT &vm_settings, size_t static_data_size);

    static std::string err_to_string(MemErr err);

    uint64_t sbrk(int64_t increment, MemErr &err);

    uint64_t get_brk() const;

    size_t get_program_space_size() const;

private:
    [[nodiscard]] bool address_in_stack(uint64_t address, size_t obj_size = 0) const noexcept;

    [[nodiscard]] bool address_in_data(uint64_t address, size_t obj_size = 0) const noexcept;

    std::vector<uint8_t> m_stack{};
    std::vector<uint8_t> m_data{};

    uint64_t m_program_addr{};
    uint64_t m_heap_addr{};
    uint64_t m_stack_addr{};

    size_t m_stack_size{};

    bool initialized = false;
};
