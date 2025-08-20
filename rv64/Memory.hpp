#pragma once
#include <cstdint>


class Memory {
public:
    template<typename T>
    T load(uint64_t address) const;
};
