#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <span>

#include "parser/ParserProcessor.hpp"

enum class MemErr {
    None = 0, SegFault = 1, NotTermStr = 2, OutOfMemory = 3, NegativeSizeOfHeap = 4, ProgramExit = 5
};

class Memory {
public:
    struct Config {
        uint64_t data_addr = 0x400000;
        uint64_t stack_addr = 0x7FFFFFF0;
        size_t stack_size = 1024 * 1024; // 1 MiB
        size_t initial_heap_size = 128;
        bool asc_stack = false;
        bool big_endian = false;
    };

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

    void init(std::span<const uint8_t> program_data, const Config &conf);

    void init(std::span<const uint8_t> program_data);

    void load_program(const ParserProcessor::ParsedInstVec &instructions);

    [[nodiscard]] const Instruction &get_instruction_at(uint64_t address, MemErr &err) const;

    [[nodiscard]] const uint64_t get_instruction_end_addr() const;

    [[nodiscard]] static std::string err_to_string(MemErr err);

    uint64_t sbrk(int64_t inc, MemErr &err);

    [[nodiscard]] uint64_t get_brk() const;

    [[nodiscard]] size_t get_program_space_size() const;

    [[nodiscard]] const Config &get_conf() const;

private:
    [[nodiscard]] bool addr_in_stack(uint64_t address, size_t obj_size = 0) const noexcept;

    [[nodiscard]] bool addr_in_data(uint64_t address, size_t obj_size = 0) const noexcept;

    [[nodiscard]] uint64_t stack_addr_to_offset(uint64_t address) const noexcept;

    [[nodiscard]] uint64_t data_addr_to_offset(uint64_t address) const noexcept;

    [[nodiscard]] constexpr uint64_t stack_end () const noexcept;

    template<typename T>
    [[nodiscard]] constexpr T loadT(std::span<const uint8_t> bin) const;

    template<typename T>
    constexpr void storeT(std::span<uint8_t> bin, T value) const;

    std::vector<uint8_t> m_stack{};
    std::vector<uint8_t> m_data{};

    uint64_t m_stack_alloc_bottom = 0;

    Config m_config{};
    uint64_t m_stack_bottom = 0;
    uint64_t m_heap_addr = 0;

    ParserProcessor::ParsedInstVec m_instructions;

    bool m_initialized = false;
};

constexpr uint64_t Memory::stack_end() const noexcept {
    return m_stack_bottom + m_config.stack_size;
}

template<typename T>
constexpr T Memory::loadT(std::span<const uint8_t> bin) const {
    T value = 0;
    size_t sz = sizeof(T);
    if (m_config.big_endian) {
        for (size_t i = 0; i < sz; ++i)
            value |= (static_cast<T>(bin[i]) << ((sz - 1 - i) * 8));
    } else {
        for (size_t i = 0; i < sz; ++i)
            value |= (static_cast<T>(bin[i]) << (i * 8));
    }
    return value;
}

template<typename T>
constexpr void Memory::storeT(std::span<uint8_t> bin, T value) const {
    size_t sz = sizeof(T);
    if (m_config.big_endian) {
        for (size_t i = 0; i < sz; ++i)
            bin[i] = static_cast<uint8_t>((value >> ((sz - 1 - i) * 8)) & 0xFF);
    } else {
        for (size_t i = 0; i < sz; ++i)
            bin[i] = static_cast<uint8_t>((value >> (i * 8)) & 0xFF);
    }
}
