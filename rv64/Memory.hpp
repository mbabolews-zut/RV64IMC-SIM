#pragma once
#include <cstdint>
#include <string>

enum class MemErr {
    None = 0, SegFault = 1, NotTermStr = 2, OutOfMemory = 3, NegativeSizeOfHeap = 4
};

class Memory {
public:
    template<typename T>
    T load(uint64_t address, MemErr &err);

    template<typename T>
    [[nodiscard]]  MemErr store(uint64_t address, T value);

    std::string load_string(uint64_t address, MemErr &err);

    static std::string err_to_string(MemErr err);

    uint64_t sbrk(int64_t increment, MemErr &err);
};
