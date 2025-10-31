#pragma once

#include <string>
#include <utility>

namespace chtholly {

enum class TokenType {
    // Single-character tokens.
    LEFT_PAREN, RIGHT_PAREN,
    LEFT_BRACE, RIGHT_BRACE,
    LEFT_BRACKET, RIGHT_BRACKET,
    COMMA, DOT, MINUS, PLUS,
    SEMICOLON, SLASH, STAR,

    // One or two character tokens.
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,
    COLON, COLON_COLON,

    // Literals.
    IDENTIFIER, STRING, NUMBER,

    // Keywords.
    AND, OR, IF, ELSE, TRUE, FALSE, FOR, FUNC,
    LET, MUT, RETURN, STRUCT, ENUM, IMPORT,
    SWITCH, CASE, DEFAULT, BREAK, FALLTHROUGH,
    PUBLIC, PRIVATE, TRAIT, IMPL,

    // Misc
    ARROW, // ->
    ERROR,

    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;

    Token(TokenType type, std::string lexeme, int line)
        : type(type), lexeme(std::move(lexeme)), line(line) {}
};

} // namespace chtholly
