#ifndef CHTHOLLY_TOKEN_H
#define CHTHOLLY_TOKEN_H

#include <string>
#include <ostream>

enum class TokenType {
    // Single-character tokens
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE, LEFT_BRACKET, RIGHT_BRACKET,
    COMMA, DOT, MINUS, PLUS, SLASH, STAR, MODULO,
    SEMICOLON, COLON, QUESTION, AMPERSAND,

    // One or two character tokens
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,

    // Logical operators
    LOGICAL_AND, LOGICAL_OR,

    // Increment, Decrement, and Assignment Operators
    PLUS_PLUS, PLUS_EQUAL,
    MINUS_MINUS, MINUS_EQUAL,
    STAR_EQUAL, SLASH_EQUAL, MODULO_EQUAL,

    // Arrows
    ARROW, FAT_ARROW,

    // Literals
    IDENTIFIER, STRING, NUMBER_INT, NUMBER_FLOAT, CHAR,

    // Keywords
    STRUCT, FUNC, LET, MUT, IF, ELSE, RETURN, TRUE, FALSE,
    SWITCH, CASE, FALLTHROUGH, BREAK,
    PUBLIC, PRIVATE, SELF, IMPL, TRAIT, IMPORT, NONE,

    // End of File
    END_OF_FILE,

    // Unknown token
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line = 0;
    int col = 0;

    friend std::ostream& operator<<(std::ostream& os, const Token& token) {
        os << "Token(type: " << static_cast<int>(token.type)
           << ", lexeme: '" << token.lexeme
           << "', line: " << token.line
           << ", col: " << token.col << ")";
        return os;
    }
};

#endif // CHTHOLLY_TOKEN_H
