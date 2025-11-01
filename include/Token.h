#ifndef CHTHOLLY_TOKEN_H
#define CHTHOLLY_TOKEN_H

#include <string>

enum class TokenType {
    // Single-character tokens
    LEFT_PAREN, RIGHT_PAREN,
    LEFT_BRACE, RIGHT_BRACE,
    LEFT_BRACKET, RIGHT_BRACKET,
    COMMA, DOT, SEMICOLON, COLON,
    QUESTION,

    // One or two character tokens
    MINUS, PLUS, SLASH, STAR, MODULO,
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,
    AMPERSAND, AMPERSAND_AMPERSAND,
    PIPE, PIPE_PIPE,

    // Literals
    IDENTIFIER, STRING, NUMBER, CHAR,

    // Keywords
    FUNC, LET, MUT, IF, ELSE, SWITCH, CASE, BREAK, FALLTHROUGH,
    FOR, WHILE, RETURN,
    STRUCT, ENUM, SELF,
    PUBLIC, PRIVATE,
    IMPORT,
    TRAIT, IMPL,
    OPTION, NONE,

    // End of file
    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
};

#endif //CHTHOLLY_TOKEN_H
