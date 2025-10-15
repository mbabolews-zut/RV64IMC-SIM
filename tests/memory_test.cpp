#include <catch2/catch_all.hpp>
#include <rv64/Memory.hpp>
#include <rv64/VM.hpp>

TEST_CASE("Memory basic tests", "[memory][load][store]") {
    rv64::VM vm{};
    Memory &mem = vm.m_memory;
    MemErr err = MemErr::None;
    const auto &conf = vm.get_settings();

    // Store to data segment
    SECTION("Storing to data segment") {
        INFO("brk:" + std::to_string(mem.get_brk()));
        INFO("program_space_size: " + std::to_string(mem.get_program_space_size()));
        err = mem.store(conf.prog_start_address + 5, 0x12345678);
        INFO(Memory::err_to_string(err));
        REQUIRE(err == MemErr::None);

        // Edge cases
        INFO("brk:" + std::to_string(mem.get_brk()));
        INFO("program_space_size: " + std::to_string(mem.get_program_space_size()));
        err = mem.store(conf.prog_start_address, 0xdeadbeef);
        INFO(Memory::err_to_string(err));
        REQUIRE(err == MemErr::None);

        INFO("brk:" + std::to_string(mem.get_brk()));
        INFO("program_space_size: " + std::to_string(mem.get_program_space_size()));
        err = mem.store<uint32_t>(mem.get_brk() - 4, 0xabcdef01);
        INFO(Memory::err_to_string(err));
        REQUIRE(err == MemErr::None);

        // Error cases
        err = mem.store<uint16_t>(conf.prog_start_address - 1, 0xbeef);
        INFO(Memory::err_to_string(err));
        REQUIRE(err == MemErr::SegFault);

        err = mem.store<int8_t>(conf.prog_start_address + Memory::PROGRAM_MEM_LIMIT - 4, 0x20);
        INFO(Memory::err_to_string(err));
        REQUIRE(err == MemErr::SegFault);

        // Load to data segment
        SECTION("Loading from data segment") {
            auto val = mem.load<uint32_t>(conf.prog_start_address + 5, err);
            INFO(Memory::err_to_string(err));
            REQUIRE(err == MemErr::None);
            REQUIRE(val == 0x12345678);

            val = mem.load<uint32_t>(conf.prog_start_address, err);
            INFO(Memory::err_to_string(err));
            REQUIRE(err == MemErr::None);
            REQUIRE(val == 0xdeadbeef);

            val = mem.load<uint32_t>(mem.get_brk() - 4, err);
            INFO(Memory::err_to_string(err));
            REQUIRE(err == MemErr::None);
            REQUIRE(val == 0xabcdef01);

            // Error cases
            mem.load<uint16_t>(conf.prog_start_address - 1, err);
            INFO(Memory::err_to_string(err));
            REQUIRE(err == MemErr::SegFault);

            mem.load<uint16_t>(conf.prog_start_address + Memory::PROGRAM_MEM_LIMIT - 4, err);
            INFO(Memory::err_to_string(err));
            REQUIRE(err == MemErr::SegFault);

            mem.load<int8_t>(mem.get_brk(), err);
            INFO(Memory::err_to_string(err));
            REQUIRE(err == MemErr::SegFault);
        }
    }

    // Store to stack segment
    SECTION("Storing to stack segment") {
        err = mem.store<uint16_t>(conf.stack_start_address - 0x1000, UINT16_MAX);
        INFO(Memory::err_to_string(err));
        REQUIRE(err == MemErr::None);

        // Edge cases
        err = mem.store<int64_t>(conf.stack_start_address - 8, INT64_MAX - 2);
        INFO(Memory::err_to_string(err));
        REQUIRE(err == MemErr::None);

        err = mem.store<int16_t>(conf.stack_start_address - conf.stack_size, INT16_MAX);
        INFO(Memory::err_to_string(err));
        REQUIRE(err == MemErr::None);

        // Error cases
        err = mem.store<uint16_t>(conf.stack_start_address - 1, 0xbeef);
        INFO(Memory::err_to_string(err));
        REQUIRE(err == MemErr::SegFault);

        err = mem.store<int8_t>(conf.stack_start_address, 0x20);
        INFO(Memory::err_to_string(err));
        REQUIRE(err == MemErr::SegFault);

        // Load to stack segment
        SECTION("Loading from stack segment") {
            auto val = mem.load<uint16_t>(conf.stack_start_address - 0x1000, err);
            INFO(Memory::err_to_string(err));
            REQUIRE(err == MemErr::None);
            REQUIRE(val == UINT16_MAX);

            // Edge cases
            val = mem.load<int16_t>(conf.stack_start_address - conf.stack_size, err);
            INFO(Memory::err_to_string(err));
            REQUIRE(err == MemErr::None);
            REQUIRE(val == INT16_MAX);

            auto val64 = mem.load<int64_t>(conf.stack_start_address - 8, err);
            INFO(Memory::err_to_string(err));
            REQUIRE(err == MemErr::None);
            REQUIRE(val64 == INT64_MAX - 2);

            // Error cases
            mem.load<uint16_t>(conf.stack_start_address - 1, err);
            INFO(Memory::err_to_string(err));
            REQUIRE(err == MemErr::SegFault);

            mem.load<int16_t>(conf.stack_start_address - conf.stack_size - 1, err);
            INFO(Memory::err_to_string(err));
            REQUIRE(err == MemErr::SegFault);

            mem.load<int8_t>(conf.stack_start_address, err);
            INFO(Memory::err_to_string(err));
            REQUIRE(err == MemErr::SegFault);
        }
    }
}


TEST_CASE("Memory sbrk behavior", "[memory][sbrk]") {
    rv64::VM vm{};
    Memory &mem = vm.m_memory;
    MemErr err = MemErr::None;

    const auto &conf = mem.get_conf();
    INFO(std::format("initial brk: {:X}, initial heap size: {:X}",
        conf.data_addr,
        conf.initial_heap_size));
    const uint64_t initial_brk = mem.get_brk();

    SECTION("Heap grows correctly") {
        auto old = mem.sbrk(64, err);
        REQUIRE(err == MemErr::None);
        REQUIRE(mem.get_brk() == initial_brk + 64);
        REQUIRE(old == initial_brk);
    }

    SECTION("Heap shrinks correctly") {
        mem.sbrk(128, err);
        REQUIRE(mem.get_brk() == initial_brk + 128);
        REQUIRE(err == MemErr::None);
        auto prev_brk = mem.sbrk(-64, err);
        REQUIRE(err == MemErr::None);
        REQUIRE(prev_brk == initial_brk + 128);
        REQUIRE(mem.get_brk() == initial_brk + 64);
    }

    SECTION("Negative shrink beyond base heap should fail") {
        auto new_brk = mem.sbrk(-conf.initial_heap_size - 4, err);
        INFO(Memory::err_to_string(err));
        REQUIRE(err == MemErr::NegativeSizeOfHeap);
    }

    SECTION("Out of memory condition") {
        // artificially grow until hitting limit
        size_t total_growth = 0;
        while (err == MemErr::None && total_growth < Memory::PROGRAM_MEM_LIMIT * 2) {
            mem.sbrk(1024 * 1024, err);
            total_growth += 1024 * 1024;
        }
        REQUIRE(err == MemErr::OutOfMemory);
    }
}