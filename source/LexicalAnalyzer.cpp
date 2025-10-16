#include "LexicalAnalyzer.hpp"

std::vector<LexicalAnalyzer::Token> LexicalAnalyzer::tokenize_line(const std::string& line) {
        std::vector<Token> tokens;

        const std::regex label_def_regex("^*[a-zA-Z_][a-zA-Z0-9_]*:$");
        const std::regex directive_regex("^*\\.[a-zA-Z_][a-zA-Z0-9_]*");
        const std::regex instruction_regex(R"(^*([a-zA-Z]+(\.[a-zA-Z]+){0,2}))");
        const std::regex identifier_regex("[a-zA-Z_][a-zA-Z0-9_]*");
        const std::regex number_regex("[-+]?(0x[0-9a-fA-F]+|0b[01]+|[0-9]+)");
        const std::regex comment_regex("[#;].*");

        std::istringstream iss(line);
        std::string word;
        bool first_token = true;

        while (iss >> word) {
            if (word[0] == '#' || word[0] == ';') {
                tokens.push_back({TokenType::Comment, line.substr(line.find(word))});
                break;
            }

            bool has_comma = false;
            if (word.back() == ',') {
                has_comma = true;
                word.pop_back();
            }

            std::smatch match;
            bool matched = false;

            if (first_token) {
                if (std::regex_match(word, match, label_def_regex)) {
                    tokens.push_back({TokenType::Label, word.substr(0, word.size() - 1)});
                    matched = true;
                } else if (std::regex_match(word, match, directive_regex)) {
                    tokens.push_back({TokenType::Directive, word});
                    matched = true;
                } else if (std::regex_match(word, match, instruction_regex)) {
                    tokens.push_back({TokenType::Instruction, word});
                    matched = true;
                }
                first_token = false;
            }

            if (!matched) {
                if (std::regex_match(word, match, number_regex)) {
                    tokens.push_back({TokenType::Number, word});
                } else if (std::regex_match(word, match, identifier_regex)) {
                    tokens.push_back({TokenType::Identifier, word});
                } else if (!word.empty()) {
                    tokens.push_back({TokenType::SyntaxError, word});
                }
            }

            if (word == ")") tokens.push_back({TokenType::RightParen, ")"});
            if (word == "(") tokens.push_back({TokenType::LeftParen, "("});
            if (has_comma) tokens.push_back({TokenType::Comma, ","});
        }
        return tokens;
    }