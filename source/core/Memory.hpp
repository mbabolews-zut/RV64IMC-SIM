#pragma once
#include <string>
#include <span>
#include <bit>
#include <memory>
#include <optional>

#include "PagedMemory.hpp"
#include "parser/asm_parsing.hpp"

enum class MemErr {
    None = 0,
    SegFault = 1,
    NotTermStr = 2,
    OutOfMemory = 3,
    NegativeSizeOfHeap = 4,
    InvalidInstructionAddress = 5, ///< i.e. padding fetch
    ProgramExit = 6,
};

class Memory {
public:
    static constexpr size_t PROGRAM_MEM_LIMIT = 1024 * 1024 * 8; // 8 MiB
    static constexpr size_t DEFAULT_STACK_SIZE = 1024 * 1024;    // 1 MiB
    static constexpr size_t DEFAULT_INITIAL_HEAP = 128;

    struct Layout {
        uint64_t data_base;
        uint64_t stack_base;
        size_t stack_size;
        size_t initial_heap_size;
        std::endian endianness;

        explicit Layout(
            uint64_t data = 0x400000,
            uint64_t stack = 0x7FF00000,
            size_t stack_sz = DEFAULT_STACK_SIZE,
            size_t heap_sz = DEFAULT_INITIAL_HEAP,
            std::endian endian = std::endian::little)
            : data_base(data)
            , stack_base(stack)
            , stack_size(stack_sz)
            , initial_heap_size(heap_sz)
            , endianness(endian) {}
    };

public:
    explicit Memory(const Layout &layout, std::span<const uint8_t> program_data = {});
    Memory& operator=(Memory&&) noexcept = default;

    [[nodiscard]] static std::string err_to_string(MemErr err);

    template<std::integral T>
    [[nodiscard]] T load(uint64_t address, MemErr &err) const;

    [[nodiscard]] MemErr store(uint64_t address, std::integral auto value);

    [[nodiscard]] std::string load_string(uint64_t address, MemErr &err) const;

    void load_program(const asm_parsing::ParsedInstVec &instructions);

    // Return a copy of the instruction and optional source-line mapping.
    struct InstructionFetch {
        Instruction inst;
        std::optional<size_t> lineno;
    };

    [[nodiscard]] InstructionFetch get_instruction_at(uint64_t address, MemErr &err) const;

    [[nodiscard]] uint64_t get_instruction_end_addr() const;

    uint64_t sbrk(int64_t inc, MemErr &err);
    [[nodiscard]] uint64_t get_brk() const;
    [[nodiscard]] size_t get_data_size() const;
    [[nodiscard]] const Layout &get_layout() const;

private:
    [[nodiscard]] bool in_stack(uint64_t address, size_t obj_size = 0) const noexcept;
    [[nodiscard]] bool in_data(uint64_t address, size_t obj_size = 0) const noexcept;
    [[nodiscard]] uint64_t to_stack_offset(uint64_t address) const noexcept;
    [[nodiscard]] uint64_t to_data_offset(uint64_t address) const noexcept;
    /// @brief Get address of stack end (exclusive upper bound)
    [[nodiscard]] uint64_t stack_end_addr() const noexcept;

private:
    Layout m_layout;
    uint64_t m_stack_bottom;
    uint64_t m_heap_start;
    size_t m_data_size;

    PagedMemory m_stack;
    PagedMemory m_data;

    asm_parsing::ParsedInstVec m_instructions;
};
