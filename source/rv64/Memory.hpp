#pragma once
#include <string>
#include <span>
#include <bit>
#include <memory>

#include "PagedMemory.hpp"
#include "parser/asm_parsing.hpp"

/// @brief Memory operation error codes
enum class MemErr {
    None = 0, ///< No error occurred
    SegFault = 1, ///< Segmentation fault - invalid memory access
    NotTermStr = 2, ///< String is not null-terminated
    OutOfMemory = 3, ///< Allocation exceeded memory limit
    NegativeSizeOfHeap = 4, ///< Heap shrunk below program data
    ProgramExit = 5, ///< Program counter reached end of instructions
    UnalignedAccess = 6 ///< Unaligned memory access (RISC-V requires alignment)
};

/// @brief Memory subsystem managing stack, heap, and program data using paged memory allocation
///
/// This class implements a complete memory subsystem for the RISC-V simulator, supporting:
/// - Separate stack and data/heap segments with configurable layout
/// - Paged memory allocation for efficient memory usage
/// - Configurable endianness (little/big/native)
/// - Dynamic heap management via sbrk system call
class Memory {
public:
    static constexpr size_t PROGRAM_MEM_LIMIT = 1024 * 1024 * 8; ///< Maximum program data size (8 MiB)
    static constexpr size_t DEFAULT_STACK_SIZE = 1024 * 1024; ///< Default stack size (1 MiB)
    static constexpr size_t DEFAULT_INITIAL_HEAP = 128; ///< Default initial heap size

    /// @brief Memory layout configuration structure
    ///
    /// Defines the virtual memory layout including base addresses, sizes,
    /// and byte ordering. Stack can be accessed in any direction.
    struct Layout {
        uint64_t data_base; ///< Base address of data/heap segment
        uint64_t stack_base; ///< Base address of stack segment
        size_t stack_size; ///< Total stack size in bytes
        size_t initial_heap_size; ///< Initial heap size in bytes
        std::endian endianness; ///< Byte ordering for multi-byte values

        /// @brief Construct a memory layout with optional custom parameters
        /// @param data Base address for program data/heap segment (default: 0x400000)
        /// @param stack Stack base address (default: 0x7FF00000)
        /// @param stack_sz Stack size in bytes (default: 1 MiB)
        /// @param heap_sz Initial heap size (default: 128 bytes)
        /// @param endian Byte ordering (default: little-endian)
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
              , endianness(endian) {
        }
    };

public:
    /// @brief Construct memory subsystem with given layout and optional program data
    /// @param layout Memory layout configuration
    /// @param program_data Initial program data to load into memory (optional)
    explicit Memory(const Layout &layout, std::span<const uint8_t> program_data = {});

    /// @brief Load a typed value from memory
    /// @tparam T Integral type to load (e.g., uint8_t, int32_t, uint64_t)
    /// @param address Virtual address to read from
    /// @param err Reference to error code (set to MemErr::None on success)
    /// @return Value read from memory (0 on error)
    template<std::integral T>
    T load(uint64_t address, MemErr &err) const;

    /// @brief Store a typed value to memory
    /// @tparam T Integral type to store (e.g., uint8_t, int32_t, uint64_t)
    /// @param address Virtual address to write to
    /// @param value Value to write
    /// @return Error code (MemErr::None on success)
    template<std::integral T>
    [[nodiscard]] MemErr store(uint64_t address, T value);

    /// @brief Load null-terminated string from memory
    /// @param address Virtual address of string start
    /// @param err Reference to error code
    /// @return String contents (empty on error)
    /// @note Returns error if string exceeds 4096 bytes or is not null-terminated
    std::string load_string(uint64_t address, MemErr &err) const;

    /// @brief Load program instructions into memory
    /// @param instructions Vector of parsed instructions with line numbers
    /// @note Should be called after construction to enable instruction fetching
    void load_program(const asm_parsing::ParsedInstVec &instructions);

    /// @brief Fetch instruction at given address
    /// @param address Virtual address to fetch instruction from
    /// @param err Reference to error code
    /// @param line Optional pointer to receive source line number
    /// @return Reference to instruction (invalid instruction on error)
    [[nodiscard]] const Instruction &get_instruction_at(uint64_t address, MemErr &err, size_t *line = nullptr) const;

    /// @brief Get address immediately after last instruction
    /// @return Address following the last instruction
    [[nodiscard]] uint64_t get_instruction_end_addr() const;

    /// @brief Convert error code to human-readable string
    /// @param err Error code to convert
    /// @return Descriptive error message
    [[nodiscard]] static std::string err_to_string(MemErr err);

    /// @brief Adjust program break (heap boundary) by increment
    /// @param inc Number of bytes to grow (positive) or shrink (negative) heap
    /// @param err Reference to error code
    /// @return Previous program break address (0 on error)
    /// @note Implements RISC-V sbrk system call semantics
    uint64_t sbrk(int64_t inc, MemErr &err);

    /// @brief Get current program break address
    /// @return Address of the first byte after the heap
    [[nodiscard]] uint64_t get_brk() const;

    /// @brief Get total size of data segment (including heap)
    /// @return Size in bytes of program data plus heap
    [[nodiscard]] size_t get_data_size() const;

    /// @brief Get memory layout configuration
    /// @return Const reference to layout structure
    [[nodiscard]] const Layout &get_layout() const;

private:
    /// @brief Check if address range is within stack segment
    [[nodiscard]] bool in_stack(uint64_t address, size_t obj_size = 0) const noexcept;
    /// @brief Check if address range is within data segment
    [[nodiscard]] bool in_data(uint64_t address, size_t obj_size = 0) const noexcept;
    /// @brief Convert virtual stack address to offset in stack memory
    [[nodiscard]] uint64_t to_stack_offset(uint64_t address) const noexcept;
    /// @brief Convert virtual data address to offset in data memory
    [[nodiscard]] uint64_t to_data_offset(uint64_t address) const noexcept;
    /// @brief Get address of stack end (exclusive upper bound)
    [[nodiscard]] uint64_t stack_end_addr() const noexcept;

private:
    Layout m_layout; ///< Memory layout configuration
    uint64_t m_stack_bottom; ///< Lowest valid stack address
    uint64_t m_heap_start; ///< Address where heap begins (after program data)
    size_t m_data_size; ///< Total size of data segment including heap

    std::unique_ptr<PagedMemory> m_stack; ///< Paged memory for stack segment
    std::unique_ptr<PagedMemory> m_data; ///< Paged memory for data/heap segment

    asm_parsing::ParsedInstVec m_instructions; ///< Loaded program instructions
};
