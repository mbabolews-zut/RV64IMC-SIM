#include "Memory.hpp"

std::string Memory::err_to_string(MemErr err) {
    switch (err) {
        case MemErr::None:
            return "No error";
        case MemErr::OutOfBounds:
            return "Out of bounds";
        default:
            return "Unknown error";
    }
}
