#ifndef CHTHOLLY_TOKEN_H
#define CHTHOLLY_TOKEN_H

#include <string>

enum class TokenType {
    // Single-character tokens
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE, LEFT_BRACKET, RIGHT_BRACKET,
    COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR, QUESTION, AMPERSAND, COLON,

    // One or two character tokens
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,
    ARROW, COLON_COLON,

    // Literals
    IDENTIFIER, STRING, NUMBER, CHAR,

    // Keywords
    IF, ELSE, FUNC, RESULT, PASS, FAIL, IS_PASS, IS_FAIL, LET, MUT, OPTION, NONE,
    UNWARP, UNWARP_OR, INT, INT8, INT16, INT32, INT64, UINT8, UINT16,
    UINT32, UINT64, CHAR_TYPE, DOUBLE, FLOAT, LONG, VOID, BOOL,
    STRING_TYPE, ARRAY, STRUCT, ENUM, RETURN, PRIVATE, PUBLIC, SELF,
    SWITCH, CASE, BREAK, FALLTHROUGH, TRAIT, IMPL, IMPORT, TYPE_CAST,

    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;

    Token(TokenType type, const std::string& lexeme, int line);

    std::string toString() const;
};

#endif //CHTHOLLY_TOKEN_H
