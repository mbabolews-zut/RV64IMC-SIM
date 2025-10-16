#pragma once
#include <string>
#include <vector>
#include <regex>

class LexicalAnalyzer {
public:
    enum class TokenType {
        SyntaxError,
        Identifier,
        Instruction,
        Directive,
        Number,
        Comma,
        Comment,
        LeftParen,
        RightParen,
        Label
    };

    struct Token {
        TokenType type;
        std::string value;
    };

    static std::vector<Token> tokenize_line(const std::string& line);
};
