#pragma once

#include <string>
#include <variant>

enum class TokenType {
    // Single-character tokens.
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    LEFT_BRACKET, RIGHT_BRACKET,
    COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR, COLON, AMPERSAND,

    // One or two character tokens.
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,
    ARROW,
    COLON_COLON,

    // Literals.
    IDENTIFIER, STRING, NUMBER,

    // Keywords.
    LET, MUT, TRUE, FALSE, IF, ELSE, WHILE, FUNC, FUNCTION, RETURN, STRUCT,
    TRAIT, IMPL, FOR, IMPORT, SWITCH, CASE, BREAK, DEFAULT, FALLTHROUGH, ENUM,

    // Logical operators
    AMPERSAND_AMPERSAND, PIPE_PIPE,

    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string lexeme;
    std::variant<std::monostate, std::string, double, bool> literal;
    int line;
};
