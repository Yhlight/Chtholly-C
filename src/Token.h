#pragma once

#include <string>
#include <variant>

namespace chtholly {

enum class TokenType {
    // Single-character tokens.
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    LEFT_BRACKET, RIGHT_BRACKET,
    COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,
    QUESTION, COLON,

    // One or two character tokens.
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,
    PLUS_EQUAL, MINUS_EQUAL, STAR_EQUAL, SLASH_EQUAL,
    AMPERSAND, AMPERSAND_AMPERSAND, PIPE, PIPE_PIPE,

    // Literals.
    IDENTIFIER, STRING, NUMBER,

    // Keywords.
    LET, MUT, FUNC, STRUCT, ENUM, IF, ELSE, SWITCH, CASE, FALLTHROUGH, BREAK,
    WHILE, RETURN, TRUE, FALSE, NONE, OPTION, RESULT, PASS, FAIL, IS_PASS, IS_FAIL,
    IMPORT, TYPE_CAST, REFLECT, META, UTIL, SELF, PUBLIC, PRIVATE, TRAIT, IMPL,

    // For lambda expressions
    ARROW,

    ERROR,
    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string lexeme;
    std::variant<std::string, double, bool, nullptr_t> literal;
    int line;
};

} // namespace chtholly
