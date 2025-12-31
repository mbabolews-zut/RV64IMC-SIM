#include <PagedMemory.hpp>
#include <algorithm>
#include <array>
#include <cassert>
#include <type_traits>
#include <endianness.hpp>
#include <common.hpp>

PagedMemory::Iterator::Iterator() noexcept = default;

PagedMemory::Iterator::Iterator(const Iterator &) noexcept = default;

PagedMemory::Iterator::Iterator(Iterator &&) noexcept = default;

PagedMemory::Iterator &PagedMemory::Iterator::operator=(const Iterator &) noexcept = default;

PagedMemory::Iterator &PagedMemory::Iterator::operator=(Iterator &&) noexcept = default;

PagedMemory::Iterator::Iterator(PagedMemory *paged_memory, size_t pos) : m_pos(pos), m_paged_mem(paged_memory) {
}

PagedMemory::PagedMemory(size_t memory_size, std::endian endianness) : m_page_table(memory_size / PageSize + 1),
                                                                       m_endianness(endianness),
                                                                       m_mem_size(memory_size) {
}

bool PagedMemory::store(uint64_t addr, std::integral auto val) noexcept {
    if (addr + sizeof(val) > size())
        return false;

    if (m_endianness == std::endian::little)
        val = endianness::swap_on_be_platform(val);
    else if (m_endianness == std::endian::big)
        val = endianness::swap_on_le_platform(val);

    try {
        std::copy_n(reinterpret_cast<uint8_t *>(&val), sizeof(val), begin() + ptrdiff_t(addr));
        return true;
    } catch (...) {
        return false;
    }
}

bool PagedMemory::load(uint64_t addr, std::integral auto &val) const noexcept {
    using T = std::remove_reference_t<decltype(val)>;
    if (addr + sizeof(T) > size())
        return false;

    std::array<uint8_t, sizeof(T)> tmp{};
    for (size_t i = 0; i < sizeof(T); ++i) {
        tmp[i] = read_byte(addr + i);
    }
    val = std::bit_cast<T>(tmp);

    if (m_endianness == std::endian::little)
        val = endianness::swap_on_be_platform(val);
    else if (m_endianness == std::endian::big)
        val = endianness::swap_on_le_platform(val);

    return true;
}

PagedMemory::Iterator PagedMemory::begin() noexcept { return {this, 0}; }

PagedMemory::Iterator PagedMemory::end() noexcept { return {this, m_mem_size}; }

size_t PagedMemory::size() const noexcept { return m_mem_size; }

uint8_t PagedMemory::operator[](uint64_t addr) {
    if (addr >= size()) {
        throw std::out_of_range("PagedMemory: address out of range");
    }
    auto page = which_page_w_alloc(addr);
    return m_page_table[page][addr % PageSize];
}

uint8_t PagedMemory::read_byte(uint64_t addr) const noexcept {
    if (addr >= size()) return 0;
    auto page = which_page(addr);
    if (m_page_table[page] == nullptr) return 0;
    return m_page_table[page][addr % PageSize];
}

size_t PagedMemory::which_page(uint64_t addr) noexcept {
    return addr / PageSize;
}

size_t PagedMemory::which_page_w_alloc(uint64_t addr) noexcept {
    assert(addr < size());
    auto page = which_page(addr);
    if (m_page_table[page] == nullptr) {
        m_page_table[page] = std::make_unique<uint8_t[]>(PageSize);
    }
    return page;
}

size_t PagedMemory::page_count() const noexcept {
    return m_page_table.size();
}

PagedMemory::Iterator &PagedMemory::Iterator::operator++() {
    ++m_pos;
    return *this;
}

PagedMemory::Iterator PagedMemory::Iterator::operator++(int) {
    Iterator tmp = *this;
    ++m_pos;
    return tmp;
}

PagedMemory::Iterator &PagedMemory::Iterator::operator--() {
    --m_pos;
    return *this;
}

PagedMemory::Iterator PagedMemory::Iterator::operator--(int) {
    Iterator tmp = *this;
    --m_pos;
    return tmp;
}

PagedMemory::Iterator &PagedMemory::Iterator::operator+=(difference_type n) {
    m_pos += n;
    return *this;
}

PagedMemory::Iterator &PagedMemory::Iterator::operator-=(difference_type n) {
    m_pos -= n;
    return *this;
}

PagedMemory::Iterator::reference PagedMemory::Iterator::operator*() const {
    if (!m_paged_mem) {
        throw std::runtime_error("Iterator: null paged_memory pointer");
    }
    if (m_pos >= m_paged_mem->size()) {
        throw std::out_of_range("Iterator: position out of range");
    }

    auto page = m_paged_mem->which_page_w_alloc(m_pos);
    return m_paged_mem->m_page_table[page][m_pos % PageSize];
}

PagedMemory::Iterator::pointer PagedMemory::Iterator::operator->() const {
    return &operator*();
}

PagedMemory::Iterator::reference PagedMemory::Iterator::operator[](difference_type diff) const {
    if (!m_paged_mem) {
        throw std::runtime_error("Iterator: null paged_memory pointer");
    }

    size_t target_pos = m_pos + diff;
    if (target_pos >= m_paged_mem->size()) {
        throw std::out_of_range("Iterator: subscript out of range");
    }

    auto page = m_paged_mem->which_page_w_alloc(target_pos);
    return m_paged_mem->m_page_table[page][target_pos % PageSize];
}

bool PagedMemory::Iterator::operator==(const Iterator &other) const noexcept {
    return m_pos == other.m_pos;
}

bool PagedMemory::Iterator::operator<=(const Iterator &other) const noexcept {
    return m_pos <= other.m_pos;
}

bool PagedMemory::Iterator::operator>=(const Iterator &other) const noexcept {
    return m_pos >= other.m_pos;
}

bool PagedMemory::Iterator::operator<(const Iterator &other) const noexcept {
    return m_pos < other.m_pos;
}

bool PagedMemory::Iterator::operator>(const Iterator &other) const noexcept {
    return m_pos > other.m_pos;
}

PagedMemory::Iterator operator+(PagedMemory::Iterator it, PagedMemory::Iterator::difference_type n) {
    it.m_pos += n;
    return it;
}

PagedMemory::Iterator operator+(PagedMemory::Iterator::difference_type n, PagedMemory::Iterator it) {
    it.m_pos += n;
    return it;
}

PagedMemory::Iterator operator-(PagedMemory::Iterator it, PagedMemory::Iterator::difference_type n) {
    it.m_pos -= n;
    return it;
}

PagedMemory::Iterator::difference_type operator-(const PagedMemory::Iterator &a, const PagedMemory::Iterator &b) {
    return static_cast<PagedMemory::Iterator::difference_type>(a.m_pos) - static_cast<
               PagedMemory::Iterator::difference_type>(b.m_pos);
}

#define INSTANTIATE_STORE(T) template bool PagedMemory::store(uint64_t addr, T) noexcept;
#define INSTANTIATE_LOAD(T) template bool PagedMemory::load(uint64_t addr, T&) const noexcept;
FOR_EACH_INT(INSTANTIATE_STORE)
FOR_EACH_INT(INSTANTIATE_LOAD)
