#pragma once

#include <string>
#include <variant>

namespace chtholly {

enum class TokenType {
    // Single-character tokens.
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,
    QUESTION, COLON, AMPERSAND,

    // One or two character tokens.
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,

    // Literals.
    IDENTIFIER, STRING, NUMBER,

    // Keywords.
    LET, MUT, FUNC, RETURN, IF, ELSE, WHILE, FOR, SWITCH, CASE, BREAK, FALLTHROUGH,
    STRUCT, IMPL, TRAIT, ENUM, IMPORT, NONE, TRUE, FALSE, SELF,
    ARRAY, INT, UINT8, UINT16, UINT32, UINT64, INT8, INT16, INT32, INT64,
    CHAR, DOUBLE, FLOAT, LONG_DOUBLE, VOID, BOOL,
    RESULT, OPTION, PUBLIC, PRIVATE,
    PASS, FAIL, IS_PASS, IS_FAIL,
    TYPE_CAST, OPERATOR, REFLECT, META, UTIL,

    ERROR,
    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string lexeme;
    std::variant<std::monostate, std::string, double, bool> literal;
    int line;

    std::string toString() const {
        // This is primarily for debugging purposes.
        std::string literal_str = " ";
        if (std::holds_alternative<std::string>(literal)) {
            literal_str = std::get<std::string>(literal);
        } else if (std::holds_alternative<double>(literal)) {
            literal_str = std::to_string(std::get<double>(literal));
        } else if (std::holds_alternative<bool>(literal)) {
            literal_str = std::get<bool>(literal) ? "true" : "false";
        }
        return "Token[type=" + std::to_string(static_cast<int>(type)) +
               ", lexeme='" + lexeme + "', literal=" + literal_str +
               ", line=" + std::to_string(line) + "]";
    }
};

} // namespace chtholly
