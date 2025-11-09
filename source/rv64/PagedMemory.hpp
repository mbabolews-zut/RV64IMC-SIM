#pragma once
#include <cstdint>
#include <vector>
#include <bit>
#include <memory>
#include <iterator>

/**
 * @brief Page-based memory allocation with lazy page allocation
 *
 * Implements a memory subsystem that allocates memory in pages only when accessed.
 * This provides efficient memory usage for sparse address spaces while supporting
 * random access through a custom iterator interface. Handles endianness conversion
 * transparently for multi-byte values.
 */
class PagedMemory {
    static constexpr size_t PageSize = 4096; ///< Size of each memory page in bytes

public:
    /**
     * @brief Random access iterator for paged memory
     *
     * Provides transparent access to paged memory as if it were a contiguous array.
     * Pages are allocated on-demand when dereferenced.
     */
    struct Iterator {
        using iterator_category = std::random_access_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = uint8_t;
        using pointer = value_type*;
        using reference = value_type&;

        Iterator() noexcept;
        Iterator(const Iterator&) noexcept;
        Iterator(Iterator&&) noexcept;
        Iterator& operator=(const Iterator&) noexcept;
        Iterator& operator=(Iterator&&) noexcept;

        /// Pre-increment
        Iterator& operator++();
        /// Post-increment
        Iterator operator++(int);
        /// Pre-decrement
        Iterator& operator--();
        /// Post-decrement
        Iterator operator--(int);

        /// Advance iterator by n positions
        Iterator& operator+=(difference_type n);
        /// Move iterator back by n positions
        Iterator& operator-=(difference_type n);

        /// Add offset to iterator
        friend Iterator operator+(Iterator it, difference_type n);
        /// Add offset to iterator (reverse order)
        friend Iterator operator+(difference_type n, Iterator it);
        /// Subtract offset from iterator
        friend Iterator operator-(Iterator it, difference_type n);
        /// Calculate distance between iterators
        friend difference_type operator-(const Iterator& a, const Iterator& b);

        /// Dereference iterator
        reference operator*() const;
        /// Member access
        pointer operator->() const;
        /// Subscript operator
        reference operator[](difference_type diff) const;

        /// Equality comparison
        bool operator==(const Iterator& other) const noexcept;
        /// Less than or equal comparison
        bool operator<=(const Iterator& other) const noexcept;
        /// Greater than or equal comparison
        bool operator>=(const Iterator& other) const noexcept;
        /// Less than comparison
        bool operator<(const Iterator& other) const noexcept;
        /// Greater than comparison
        bool operator>(const Iterator& other) const noexcept;

    private:
        friend class PagedMemory;
        Iterator(PagedMemory* paged_memory, size_t pos);

        size_t m_pos = 0;              ///< Current position in memory
        PagedMemory* m_paged_mem = nullptr; ///< Pointer to owning PagedMemory
    };

    static_assert(std::random_access_iterator<Iterator>);

public:
    /**
     * @brief Construct paged memory with specified size and endianness
     * @param memory_size Total virtual memory size in bytes
     * @param endianness Byte ordering for multi-byte values (default: native)
     */
    explicit PagedMemory(size_t memory_size, std::endian endianness = std::endian::native);

    /**
     * @brief Store integral value at address with endianness conversion
     * @tparam T Integral type to store
     * @param addr Virtual address to write to
     * @param val Value to store
     * @return true on success, false if address out of bounds
     * @note Automatically allocates page if not yet allocated
     */
    [[nodiscard]] bool store(uint64_t addr, std::integral auto val) noexcept;

    /**
     * @brief Load integral value from address with endianness conversion
     * @tparam T Integral type to load
     * @param addr Virtual address to read from
     * @param val Reference to receive loaded value
     * @return true on success, false if address out of bounds
     * @note Automatically allocates page if not yet allocated
     */
    template<std::integral T>
    [[nodiscard]] bool load(uint64_t addr, T& val) noexcept;

    /**
     * @brief Get iterator to beginning of memory
     * @return Iterator pointing to first byte
     */
    Iterator begin() noexcept;

    /**
     * @brief Get iterator to end of memory
     * @return Iterator pointing past last byte
     */
    Iterator end() noexcept;

    /**
     * @brief Get total virtual memory size
     * @return Size in bytes
     */
    [[nodiscard]] size_t size() const noexcept;

    /**
     * @brief Access byte at address (with bounds checking)
     * @param addr Virtual address to access
     * @return Reference to byte at address
     * @throws std::out_of_range if address is out of bounds
     * @note Allocates page if not yet allocated
     */
    [[nodiscard]] uint8_t operator[](uint64_t addr);

private:
    friend struct Iterator;

    /// Calculate which page contains given address
    [[nodiscard]] static size_t which_page(uint64_t addr) noexcept;

    /// Get page index for address, allocating if necessary
    [[nodiscard]] size_t which_page_w_alloc(uint64_t addr) noexcept;

    /// Get total number of pages in page table
    [[nodiscard]] size_t page_count() const noexcept;

    std::vector<std::unique_ptr<uint8_t[]>> m_page_table; ///< Sparse page table
    std::endian m_endianness = std::endian::little;       ///< Byte ordering
    const size_t m_mem_size;                              ///< Total virtual memory size
};