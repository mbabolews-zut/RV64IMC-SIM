#include <catch2/catch_all.hpp>
#include <rv64/Memory.hpp>
#include <rv64/VM.hpp>

TEST_CASE("Memory basic tests", "[memory]") {
    rv64::VM vm{};
    Memory &mem = vm.m_memory;
    MemErr err = MemErr::None;
    const auto &settings = vm.get_settings();

    // Store to data segment
    SECTION("Storing to data segment") {
        err = mem.store(settings.prog_start_address + 5, 0x12345678);
        INFO(Memory::err_to_string(err));
        REQUIRE(err == MemErr::None);

        // Edge cases
        err = mem.store(settings.prog_start_address, 0xdeadbeef);
        INFO(Memory::err_to_string(err));
        REQUIRE(err == MemErr::None);

        err = mem.store<uint32_t>(mem.get_brk() - 4, 0xabcdef01);
        INFO(Memory::err_to_string(err));
        REQUIRE(err == MemErr::None);

        // Error cases
        err = mem.store<uint16_t>(settings.prog_start_address - 1, 0xbeef);
        INFO(Memory::err_to_string(err));
        REQUIRE(err == MemErr::SegFault);

        err = mem.store<int8_t>(settings.prog_start_address + Memory::PROGRAM_MEM_LIMIT - 4, 0x20);
        INFO(Memory::err_to_string(err));
        REQUIRE(err == MemErr::SegFault);

        // Load to data segment
        SECTION("Loading from data segment") {
            auto val = mem.load<uint32_t>(settings.prog_start_address + 5, err);
            INFO(Memory::err_to_string(err));
            REQUIRE(err == MemErr::None);
            REQUIRE(val == 0x12345678);

            val = mem.load<uint32_t>(settings.prog_start_address, err);
            INFO(Memory::err_to_string(err));
            REQUIRE(err == MemErr::None);
            REQUIRE(val == 0xdeadbeef);

            val = mem.load<uint32_t>(mem.get_brk() - 4, err);
            INFO(Memory::err_to_string(err));
            REQUIRE(err == MemErr::None);
            REQUIRE(val == 0xabcdef01);

            // Error cases
            mem.load<uint16_t>(settings.prog_start_address - 1, err);
            INFO(Memory::err_to_string(err));
            REQUIRE(err == MemErr::SegFault);

            mem.load<uint16_t>(settings.prog_start_address + Memory::PROGRAM_MEM_LIMIT - 4, err);
            INFO(Memory::err_to_string(err));
            REQUIRE(err == MemErr::SegFault);

            mem.load<int8_t>(mem.get_brk(), err);
            INFO(Memory::err_to_string(err));
            REQUIRE(err == MemErr::SegFault);
        }
    }

    // Store to stack segment
    SECTION("Storing to stack segment") {
        err = mem.store<uint16_t>(settings.stack_start_address - 0x1000, UINT16_MAX);
        INFO(Memory::err_to_string(err));
        REQUIRE(err == MemErr::None);

        // Edge cases
        err = mem.store<int64_t>(settings.stack_start_address - 8, INT64_MAX - 2);
        INFO(Memory::err_to_string(err));
        REQUIRE(err == MemErr::None);

        err = mem.store<int16_t>(settings.stack_start_address - settings.stack_size, INT16_MAX);
        INFO(Memory::err_to_string(err));
        REQUIRE(err == MemErr::None);

        // Error cases
        err = mem.store<uint16_t>(settings.stack_start_address - 1, 0xbeef);
        INFO(Memory::err_to_string(err));
        REQUIRE(err == MemErr::SegFault);

        err = mem.store<int8_t>(settings.stack_start_address, 0x20);
        INFO(Memory::err_to_string(err));
        REQUIRE(err == MemErr::SegFault);

        // Load to stack segment
        SECTION("Loading from stack segment") {
            auto val = mem.load<uint16_t>(settings.stack_start_address - 0x1000, err);
            INFO(Memory::err_to_string(err));
            REQUIRE(err == MemErr::None);
            REQUIRE(val == UINT16_MAX);

            // Edge cases
            val = mem.load<int16_t>(settings.stack_start_address - settings.stack_size, err);
            INFO(Memory::err_to_string(err));
            REQUIRE(err == MemErr::None);
            REQUIRE(val == INT16_MAX);

            auto val64 = mem.load<int64_t>(settings.stack_start_address - 8, err);
            INFO(Memory::err_to_string(err));
            REQUIRE(err == MemErr::None);
            REQUIRE(val64 == INT64_MAX - 2);

            // Error cases
            mem.load<uint16_t>(settings.stack_start_address - 1, err);
            INFO(Memory::err_to_string(err));
            REQUIRE(err == MemErr::SegFault);

            mem.load<int16_t>(settings.stack_start_address - settings.stack_size - 1, err);
            INFO(Memory::err_to_string(err));
            REQUIRE(err == MemErr::SegFault);

            mem.load<int8_t>(settings.stack_start_address, err);
            INFO(Memory::err_to_string(err));
            REQUIRE(err == MemErr::SegFault);
        }
    }

    SECTION("sbrk function testing") {

    }
}