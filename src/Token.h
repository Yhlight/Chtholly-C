#pragma once

#include <string>
#include <variant>

namespace chtholly {

enum class TokenType {
    // Single-character tokens
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE, LEFT_BRACKET, RIGHT_BRACKET,
    COMMA, DOT, MINUS, PLUS, SLASH, STAR, MODULO, COLON, SEMICOLON, AMPERSAND, QUESTION,

    // One or two character tokens
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,
    MINUS_MINUS, PLUS_PLUS,
    MINUS_EQUAL, PLUS_EQUAL, STAR_EQUAL, SLASH_EQUAL, MODULO_EQUAL,
    ARROW, COLON_COLON,

    // Literals
    IDENTIFIER, STRING, NUMBER,

    // Keywords
    AND, OR, IF, ELSE, TRUE, FALSE, FUNC, FOR, WHILE, RETURN,
    STRUCT, ENUM, LET, MUT, CONST,
    SWITCH, CASE, DEFAULT, BREAK, FALLTHROUGH,
    IMPORT, PUBLIC, PRIVATE,
    SELF, NONE,
    TRAIT, IMPL,
    TYPE_CAST,

    // Types
    TYPE_INT, TYPE_INT8, TYPE_INT16, TYPE_INT32, TYPE_INT64,
    TYPE_UINT, TYPE_UINT8, TYPE_UINT16, TYPE_UINT32, TYPE_UINT64,
    TYPE_FLOAT, TYPE_DOUBLE,
    TYPE_CHAR, TYPE_STRING, TYPE_BOOL, TYPE_VOID,
    TYPE_ARRAY, TYPE_OPTION, TYPE_RESULT,

    ERROR,
    END_OF_FILE
};

using Literal = std::variant<std::monostate, std::string, double, bool>;

struct Token {
    TokenType type;
    std::string lexeme;
    Literal literal;
    int line;

    std::string toString() const {
        return "Token [type=" + std::to_string(static_cast<int>(type)) +
               ", lexeme='" + lexeme + "', line=" + std::to_string(line) + "]";
    }
};

} // namespace chtholly
