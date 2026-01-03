#pragma once
#include <string>
#include <optional>
#include <format>

enum class BuildErrorKind {
    UnknownMnemonic,
    InvalidRegister,
    RegisterNotInCompressedRange,
    ImmediateOutOfRange,
    MissingArgument,
    DuplicateLabel,
    UnresolvedSymbol
};

struct BuildError {
    BuildErrorKind kind;
    std::string message;
    std::optional<size_t> line;

    [[nodiscard]] std::string format() const {
        if (line) return std::format("Line {}: {}", *line, message);
        return message;
    }
};