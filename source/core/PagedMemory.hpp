#pragma once
#include <vector>
#include <bit>
#include <memory>
#include <iterator>

class PagedMemory {
    static constexpr size_t PageSize = 4096; // Memory page size in bytes

public:
    // Random access iterator for paged memory
    struct Iterator {
        using iterator_category = std::random_access_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = uint8_t;
        using pointer = value_type *;
        using reference = value_type &;

        Iterator() noexcept;
        Iterator(const Iterator &) noexcept;
        Iterator(Iterator &&) noexcept;
        Iterator &operator=(const Iterator &) noexcept;
        Iterator &operator=(Iterator &&) noexcept;

        Iterator &operator++();
        Iterator operator++(int);
        Iterator &operator--();
        Iterator operator--(int);

        Iterator &operator+=(difference_type n);
        Iterator &operator-=(difference_type n);

        friend Iterator operator+(Iterator it, difference_type n);
        friend Iterator operator+(difference_type n, Iterator it);
        friend Iterator operator-(Iterator it, difference_type n);
        friend difference_type operator-(const Iterator &a, const Iterator &b);

        reference operator*() const;
        pointer operator->() const;
        reference operator[](difference_type diff) const;

        bool operator==(const Iterator &other) const noexcept;
        bool operator<=(const Iterator &other) const noexcept;
        bool operator>=(const Iterator &other) const noexcept;
        bool operator<(const Iterator &other) const noexcept;
        bool operator>(const Iterator &other) const noexcept;

    private:
        friend class PagedMemory;
        Iterator(PagedMemory *paged_memory, size_t pos);

        size_t m_pos = 0;
        PagedMemory *m_paged_mem = nullptr;
    };

    static_assert(std::random_access_iterator<Iterator>);

public:
    explicit PagedMemory(size_t memory_size, std::endian endianness = std::endian::native);

    [[nodiscard]] bool store(uint64_t addr, std::integral auto val) noexcept;
    [[nodiscard]] bool load(uint64_t addr, std::integral auto &val) noexcept;

    Iterator begin() noexcept;
    Iterator end() noexcept;

    [[nodiscard]] size_t size() const noexcept;

    /// @brief Access byte at address (allocates page if needed)
    [[nodiscard]] uint8_t operator[](uint64_t addr);

private:
    /// @brief Calculate page index for address
    [[nodiscard]] static size_t which_page(uint64_t addr) noexcept;

    /// @brief Get or allocate page index for address and allocate the page if needed
    [[nodiscard]] size_t which_page_w_alloc(uint64_t addr) noexcept;

    [[nodiscard]] size_t page_count() const noexcept;

    std::vector<std::unique_ptr<uint8_t[]> > m_page_table;
    std::endian m_endianness = std::endian::little;
    const size_t m_mem_size; // Total memory size
};
