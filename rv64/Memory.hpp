#pragma once
#include <cstdint>
#include <string>

enum class MemErr {
    None = 0, OutOfBounds = 1
};

class Memory {
public:
    template<typename T>
    static T load(uint64_t address, MemErr &err);

    template<typename T>
    [[nodiscard]] static MemErr store(uint64_t address, T value);

    static std::string err_to_string(MemErr err);
};
