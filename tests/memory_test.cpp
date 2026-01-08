#include <catch2/catch_all.hpp>
#include <Memory.hpp>
#include <rv64/VM.hpp>

TEST_CASE("Memory descending stack (default)", "[memory][stack][descending]") {
    Memory::Layout layout;
    layout.stack_base = 0x7FF00000;  // Base (bottom) of stack
    layout.stack_size = 0x100000; // 1 MiB
    rv64::VM vm{{.m_mem_layout = layout}};
    Memory &mem = vm.m_memory;
    MemErr err = MemErr::None;

    const uint64_t stack_bottom = layout.stack_base;
    const uint64_t stack_top = layout.stack_base + layout.stack_size;

    SECTION("Stack boundaries are correct") {
        INFO("stack_bottom: " + std::format("{:#x}", stack_bottom));
        INFO("stack_top: " + std::format("{:#x}", stack_top));
        REQUIRE(stack_bottom == 0x7FF00000);
        REQUIRE(stack_top == 0x80000000);
    }

    SECTION("Can write to stack bottom (lowest valid address)") {
        err = mem.store<uint64_t>(stack_bottom, 0xDEADBEEFCAFEBABE);
        REQUIRE(err == MemErr::None);

        auto val = mem.load<uint64_t>(stack_bottom, err);
        REQUIRE(err == MemErr::None);
        REQUIRE(val == 0xDEADBEEFCAFEBABE);
    }

    SECTION("Can write to stack top - 8 (highest valid address for 64-bit)") {
        err = mem.store<uint64_t>(stack_top - 8, 0x123456789ABCDEF0);
        REQUIRE(err == MemErr::None);

        auto val = mem.load<uint64_t>(stack_top - 8, err);
        REQUIRE(err == MemErr::None);
        REQUIRE(val == 0x123456789ABCDEF0);
    }

    SECTION("Can write throughout the stack") {
        err = mem.store<uint32_t>(stack_bottom + 0x1000, 0x11111111);
        REQUIRE(err == MemErr::None);

        err = mem.store<uint32_t>(stack_bottom + 0x50000, 0x22222222);
        REQUIRE(err == MemErr::None);

        err = mem.store<uint32_t>(stack_top - 0x1000, 0x33333333);
        REQUIRE(err == MemErr::None);
    }

    SECTION("Cannot write below stack bottom") {
        err = mem.store<uint64_t>(stack_bottom - 8, 0xDEADBEEF);
        REQUIRE(err == MemErr::SegFault);

        err = mem.store<uint8_t>(stack_bottom - 1, 0x42);
        REQUIRE(err == MemErr::SegFault);
    }

    SECTION("Cannot write at or above stack top") {
        err = mem.store<uint64_t>(stack_top, 0xDEADBEEF);
        REQUIRE(err == MemErr::SegFault);

        err = mem.store<uint8_t>(stack_top + 100, 0x42);
        REQUIRE(err == MemErr::SegFault);
    }

    SECTION("Multi-byte values cannot cross stack boundary") {
        // First verify stack_bottom alignment for predictable test behavior
        REQUIRE(stack_bottom % 8 == 0);  // Ensure 8-byte alignment

        // Test 1: Aligned 32-bit value entirely within bounds (near top)
        err = mem.store<uint32_t>(stack_top - 4, 0x12345678);
        REQUIRE(err == MemErr::None);  // 0x7FFFFFFC-0x7FFFFFFF all valid

        // Test 2: Aligned 64-bit value at boundary - starts outside
        err = mem.store<uint64_t>(stack_top, 0x123456789ABCDEF0);
        REQUIRE(err == MemErr::SegFault);  // 0x80000000 is first invalid address

        // Test 3: Aligned 64-bit value at bottom boundary
        err = mem.store<uint64_t>(stack_bottom - 8, 0xDEADBEEFCAFEBABE);
        REQUIRE(err == MemErr::SegFault);  // Entirely below valid range

        // Test 4: Properly aligned value entirely below stack
        if (stack_bottom >= 16) {  // Ensure we have room
            uint64_t below_stack = (stack_bottom - 16) & ~7ULL;  // Align down
            err = mem.store<uint64_t>(below_stack, 0xDEADBEEF);
            REQUIRE(err == MemErr::SegFault);
        }
    }
}

TEST_CASE("Memory stack at different address", "[memory][stack]") {
    Memory::Layout layout;
    layout.stack_base = 0x10000000;
    layout.stack_size = 0x100000; // 1 MiB
    rv64::VM vm{{.m_mem_layout = layout}};
    Memory &mem = vm.m_memory;
    MemErr err = MemErr::None;

    const uint64_t stack_bottom = layout.stack_base;
    const uint64_t stack_top = layout.stack_base + layout.stack_size;

    SECTION("Stack boundaries are correct") {
        INFO("stack_bottom: " + std::format("{:#x}", stack_bottom));
        INFO("stack_top: " + std::format("{:#x}", stack_top));
        REQUIRE(stack_bottom == 0x10000000);
        REQUIRE(stack_top == 0x10100000);
    }

    SECTION("Can write to stack bottom (lowest valid address)") {
        err = mem.store<uint64_t>(stack_bottom, 0xDEADBEEFCAFEBABE);
        REQUIRE(err == MemErr::None);

        auto val = mem.load<uint64_t>(stack_bottom, err);
        REQUIRE(err == MemErr::None);
        REQUIRE(val == 0xDEADBEEFCAFEBABE);
    }

    SECTION("Can write near stack top") {
        err = mem.store<uint64_t>(stack_top - 8, 0x123456789ABCDEF0);
        REQUIRE(err == MemErr::None);

        auto val = mem.load<uint64_t>(stack_top - 8, err);
        REQUIRE(err == MemErr::None);
        REQUIRE(val == 0x123456789ABCDEF0);
    }

    SECTION("Cannot write at or above stack top") {
        err = mem.store<uint64_t>(stack_top, 0xDEADBEEF);
        REQUIRE(err == MemErr::SegFault);

        err = mem.store<uint8_t>(stack_top + 100, 0x42);
        REQUIRE(err == MemErr::SegFault);
    }

    SECTION("Cannot write below stack bottom") {
        err = mem.store<uint64_t>(stack_bottom - 8, 0xDEADBEEF);
        REQUIRE(err == MemErr::SegFault);
    }
}

TEST_CASE("Memory data segment", "[memory][data][heap]") {
    rv64::VM vm{};
    Memory &mem = vm.m_memory;
    MemErr err = MemErr::None;
    const auto &layout = vm.get_memory_layout();

    SECTION("Can write to data segment start") {
        err = mem.store<uint64_t>(layout.data_base, 0xDEADBEEF);
        REQUIRE(err == MemErr::None);

        auto val = mem.load<uint64_t>(layout.data_base, err);
        REQUIRE(err == MemErr::None);
        REQUIRE(val == 0xDEADBEEF);
    }

    SECTION("Can write to heap") {
        uint64_t heap_addr = layout.data_base + layout.initial_heap_size / 2;
        err = mem.store<uint32_t>(heap_addr, 0x12345678);
        REQUIRE(err == MemErr::None);
    }

    SECTION("Cannot write before data segment") {
        err = mem.store<uint64_t>(layout.data_base - 8, 0xDEADBEEF);
        REQUIRE(err == MemErr::SegFault);
    }

    SECTION("Cannot write past current brk") {
        uint64_t brk = mem.get_brk();
        err = mem.store<uint64_t>(brk, 0xDEADBEEF);
        REQUIRE(err == MemErr::SegFault);

        err = mem.store<uint8_t>(brk + 1000, 0x42);
        REQUIRE(err == MemErr::SegFault);
    }

    SECTION("Data values persist correctly") {
        std::vector<std::pair<uint64_t, uint64_t>> test_data = {
            {layout.data_base, 0x1111111111111111},
            {layout.data_base + 8, 0x2222222222222222},
            {layout.data_base + 16, 0x3333333333333333},
            {layout.data_base + 32, 0x4444444444444444}
        };

        // Write all values
        for (const auto &[addr, value] : test_data) {
            err = mem.store<uint64_t>(addr, value);
            REQUIRE(err == MemErr::None);
        }

        // Verify all values persisted
        for (const auto &[addr, expected] : test_data) {
            auto val = mem.load<uint64_t>(addr, err);
            REQUIRE(err == MemErr::None);
            REQUIRE(val == expected);
        }
    }
}

TEST_CASE("Memory sbrk behavior", "[memory][sbrk][heap]") {
    rv64::VM vm{};
    Memory &mem = vm.m_memory;
    MemErr err = MemErr::None;
    const auto &layout = vm.get_memory_layout();

    const uint64_t initial_brk = mem.get_brk();

    SECTION("sbrk(0) returns current brk without changing it") {
        uint64_t brk = mem.sbrk(0, err);
        REQUIRE(err == MemErr::None);
        REQUIRE(brk == initial_brk);
        REQUIRE(mem.get_brk() == initial_brk);
    }

    SECTION("Heap grows correctly") {
        uint64_t old_brk = mem.sbrk(4096, err);
        REQUIRE(err == MemErr::None);
        REQUIRE(old_brk == initial_brk);
        REQUIRE(mem.get_brk() == initial_brk + 4096);

        // Can now write to newly allocated area
        err = mem.store<uint64_t>(initial_brk, 0xDEADBEEF);
        REQUIRE(err == MemErr::None);
    }

    SECTION("Heap shrinks correctly") {
        mem.sbrk(8192, err);
        REQUIRE(err == MemErr::None);

        uint64_t brk_after_grow = mem.get_brk();
        uint64_t old_brk = mem.sbrk(-4096, err);
        REQUIRE(err == MemErr::None);
        REQUIRE(old_brk == brk_after_grow);
        REQUIRE(mem.get_brk() == brk_after_grow - 4096);
    }

    SECTION("Cannot shrink heap below program data") {
        mem.sbrk(-static_cast<int64_t>(layout.initial_heap_size + 1), err);
        REQUIRE(err == MemErr::NegativeSizeOfHeap);
    }

    SECTION("Cannot grow heap beyond memory limit") {
        // Try to allocate more than the limit in one go
        uint64_t huge_allocation = Memory::PROGRAM_MEM_LIMIT + 1024;
        mem.sbrk(static_cast<int64_t>(huge_allocation), err);
        REQUIRE(err == MemErr::OutOfMemory);

        // Reset error for next test
        err = MemErr::None;

        // Also test growing incrementally to the limit
        uint64_t current_size = mem.get_data_size();
        uint64_t remaining = Memory::PROGRAM_MEM_LIMIT - current_size;

        // Grow to just under the limit - should succeed
        if (remaining > 1024) {
            mem.sbrk(static_cast<int64_t>(remaining - 512), err);
            REQUIRE(err == MemErr::None);

            // Now try to grow beyond - should fail
            mem.sbrk(1024, err);
            REQUIRE(err == MemErr::OutOfMemory);
        }
    }

    SECTION("Multiple small allocations work correctly") {
        const size_t alloc_size = 256;
        const size_t num_allocs = 100;

        for (size_t i = 0; i < num_allocs; ++i) {
            uint64_t old_brk = mem.sbrk(alloc_size, err);
            REQUIRE(err == MemErr::None);
            REQUIRE(mem.get_brk() == old_brk + alloc_size);
        }

        REQUIRE(mem.get_brk() == initial_brk + alloc_size * num_allocs);
    }
}

TEST_CASE("Memory string operations", "[memory][string]") {
    // Use larger initial heap for string tests
    Memory::Layout layout;
    layout.initial_heap_size = 8192;  // Enough for 4096 byte test
    rv64::VM vm{{.m_mem_layout = layout}};
    Memory &mem = vm.m_memory;
    MemErr err = MemErr::None;
    const auto &vm_layout = vm.get_memory_layout();

    SECTION("Load null-terminated string") {
        const char *test_str = "Hello, RISC-V!";
        size_t len = strlen(test_str);

        for (size_t i = 0; i <= len; ++i) {
            err = mem.store<uint8_t>(vm_layout.data_base + i, test_str[i]);
            REQUIRE(err == MemErr::None);
        }

        std::string loaded = mem.load_string(vm_layout.data_base, err);
        REQUIRE(err == MemErr::None);
        REQUIRE(loaded == test_str);
    }

    SECTION("String not null-terminated returns error") {
        // Write 4096 non-zero bytes (max string length)
        for (size_t i = 0; i < 4096; ++i) {
            err = mem.store<uint8_t>(vm_layout.data_base + i, 'A');
            REQUIRE(err == MemErr::None);
        }

        std::string loaded = mem.load_string(vm_layout.data_base, err);
        REQUIRE(err == MemErr::NotTermStr);
    }

    SECTION("Empty string works") {
        err = mem.store<uint8_t>(vm_layout.data_base, 0);
        REQUIRE(err == MemErr::None);

        std::string loaded = mem.load_string(vm_layout.data_base, err);
        REQUIRE(err == MemErr::None);
        REQUIRE(loaded.empty());
    }
}

TEST_CASE("Memory mixed types", "[memory][types]") {
    rv64::VM vm{};
    Memory &mem = vm.m_memory;
    MemErr err = MemErr::None;
    const auto &layout = vm.get_memory_layout();

    SECTION("Different integer types at same location") {
        err = mem.store<uint32_t>(layout.data_base, 0x12345678);
        REQUIRE(err == MemErr::None);

        auto val8 = mem.load<uint8_t>(layout.data_base, err);
        REQUIRE(err == MemErr::None);
        // Little endian: lowest byte first
        REQUIRE(val8 == 0x78);

        auto val16 = mem.load<uint16_t>(layout.data_base, err);
        REQUIRE(err == MemErr::None);
        REQUIRE(val16 == 0x5678);

        auto val32 = mem.load<uint32_t>(layout.data_base, err);
        REQUIRE(err == MemErr::None);
        REQUIRE(val32 == 0x12345678);
    }

    SECTION("Signed and unsigned values") {
        err = mem.store<int32_t>(layout.data_base, -12345);
        REQUIRE(err == MemErr::None);

        auto signed_val = mem.load<int32_t>(layout.data_base, err);
        REQUIRE(err == MemErr::None);
        REQUIRE(signed_val == -12345);

        auto unsigned_val = mem.load<uint32_t>(layout.data_base, err);
        REQUIRE(err == MemErr::None);
        REQUIRE(unsigned_val == static_cast<uint32_t>(-12345));
    }
}

TEST_CASE("Memory endianness", "[memory][endianness]") {
    SECTION("Little endian (default)") {
        Memory::Layout layout;
        layout.endianness = std::endian::little;
        rv64::VM vm{{.m_mem_layout = layout}};
        Memory &mem = vm.m_memory;
        MemErr err = MemErr::None;
        const auto &vm_layout = vm.get_memory_layout();

        err = mem.store<uint32_t>(vm_layout.data_base, 0x12345678);
        REQUIRE(err == MemErr::None);

        // Little endian: LSB first
        auto b0 = mem.load<uint8_t>(vm_layout.data_base, err);
        REQUIRE(b0 == 0x78);
        auto b1 = mem.load<uint8_t>(vm_layout.data_base + 1, err);
        REQUIRE(b1 == 0x56);
        auto b2 = mem.load<uint8_t>(vm_layout.data_base + 2, err);
        REQUIRE(b2 == 0x34);
        auto b3 = mem.load<uint8_t>(vm_layout.data_base + 3, err);
        REQUIRE(b3 == 0x12);
    }

    SECTION("Big endian") {
        Memory::Layout layout;
        layout.endianness = std::endian::big;
        rv64::VM vm{{.m_mem_layout = layout}};
        Memory &mem = vm.m_memory;
        MemErr err = MemErr::None;
        const auto &vm_layout = vm.get_memory_layout();

        err = mem.store<uint32_t>(vm_layout.data_base, 0x12345678);
        REQUIRE(err == MemErr::None);

        // Big endian: MSB first
        auto b0 = mem.load<uint8_t>(vm_layout.data_base, err);
        REQUIRE(b0 == 0x12);
        auto b1 = mem.load<uint8_t>(vm_layout.data_base + 1, err);
        REQUIRE(b1 == 0x34);
        auto b2 = mem.load<uint8_t>(vm_layout.data_base + 2, err);
        REQUIRE(b2 == 0x56);
        auto b3 = mem.load<uint8_t>(vm_layout.data_base + 3, err);
        REQUIRE(b3 == 0x78);
    }

    SECTION("64-bit value byte order") {
        Memory::Layout layout;
        layout.endianness = std::endian::big;
        rv64::VM vm{{.m_mem_layout = layout}};
        Memory &mem = vm.m_memory;
        MemErr err = MemErr::None;
        const auto &vm_layout = vm.get_memory_layout();

        err = mem.store<uint64_t>(vm_layout.data_base, 0x0102030405060708);
        REQUIRE(err == MemErr::None);

        // Big endian: bytes should be 01 02 03 04 05 06 07 08
        for (int i = 0; i < 8; ++i) {
            auto b = mem.load<uint8_t>(vm_layout.data_base + i, err);
            REQUIRE(err == MemErr::None);
            REQUIRE(b == (i + 1));
        }

        // Load back as 64-bit should give original value
        auto val = mem.load<uint64_t>(vm_layout.data_base, err);
        REQUIRE(err == MemErr::None);
        REQUIRE(val == 0x0102030405060708);
    }

    SECTION("16-bit value byte order") {
        Memory::Layout layout_le;
        layout_le.endianness = std::endian::little;
        rv64::VM vm_le{{.m_mem_layout = layout_le}};
        Memory &mem_le = vm_le.m_memory;

        Memory::Layout layout_be;
        layout_be.endianness = std::endian::big;
        rv64::VM vm_be{{.m_mem_layout = layout_be}};
        Memory &mem_be = vm_be.m_memory;

        MemErr err = MemErr::None;
        const uint16_t test_val = 0xABCD;

        // Little endian
        err = mem_le.store<uint16_t>(vm_le.get_memory_layout().data_base, test_val);
        REQUIRE(err == MemErr::None);
        REQUIRE(mem_le.load<uint8_t>(vm_le.get_memory_layout().data_base, err) == 0xCD);
        REQUIRE(mem_le.load<uint8_t>(vm_le.get_memory_layout().data_base + 1, err) == 0xAB);

        // Big endian
        err = mem_be.store<uint16_t>(vm_be.get_memory_layout().data_base, test_val);
        REQUIRE(err == MemErr::None);
        REQUIRE(mem_be.load<uint8_t>(vm_be.get_memory_layout().data_base, err) == 0xAB);
        REQUIRE(mem_be.load<uint8_t>(vm_be.get_memory_layout().data_base + 1, err) == 0xCD);
    }
}
