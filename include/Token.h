#pragma once

#include <string>
#include <any>

enum class TokenType {
    // Single-character tokens.
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE, LEFT_BRACKET, RIGHT_BRACKET,
    COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR, COLON, AMPERSAND,

    // One or two character tokens.
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,

    // Literals.
    IDENTIFIER, STRING, NUMBER,

    // Keywords.
    AND, STRUCT, ELSE, FALSE, FUNC, FOR, IF, NIL, OR,
    PRINT, RETURN, SUPER, THIS, TRUE, LET, WHILE, MUT,
    PUBLIC, PRIVATE, IMPORT, ARRAY, SELF, TRAIT, IMPL,
    SWITCH, CASE, BREAK, FALLTHROUGH, ENUM,

    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string lexeme;
    std::any literal;
    int line;

    Token(TokenType type, std::string lexeme, std::any literal, int line);

    std::string toString() const;
};
