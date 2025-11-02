#pragma once

#include <string>
#include <variant>

enum class TokenType {
    // Single-character tokens.
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,
    QUESTION, COLON,

    // One or two character tokens.
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,

    // Literals.
    IDENTIFIER, STRING, NUMBER,

    // Keywords.
    AND, CLASS, ELSE, FALSE, FUNC, FOR, IF, NIL, OR,
    PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE, LET, MUT,
    STRUCT, ENUM, PUBLIC, PRIVATE, SELF, IMPORT, TRAIT, IMPL,
    SWITCH, CASE, FALLTHROUGH, NONE, BREAK, DEFAULT,

    END_OF_FILE
};

using LiteralValue = std::variant<std::monostate, std::string, double, bool>;

struct Token {
    TokenType type;
    std::string lexeme;
    LiteralValue literal;
    int line;

    Token(TokenType type, std::string lexeme, LiteralValue literal, int line)
        : type(type), lexeme(std::move(lexeme)), literal(std::move(literal)), line(line) {}

    std::string toString() const {
        // This is simplified. A real implementation would convert TokenType to string.
        return lexeme;
    }
};
