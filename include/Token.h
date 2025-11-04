#ifndef CHTHOLLY_TOKEN_H
#define CHTHOLLY_TOKEN_H

#include <string>

enum class TokenType {
    // Keywords
    FUNC,
    LET,
    MUT,
    IF,
    ELSE,
    SWITCH,
    CASE,
    DEFAULT,
    FOR,
    WHILE,
    RETURN,
    STRUCT,
    ENUM,
    TRAIT,
    IMPL,
    IMPORT,
    PUBLIC,
    PRIVATE,
    SELF,

    // Types
    INT,
    DOUBLE,
    CHAR,
    BOOL,
    STRING,
    VOID,
    ARRAY,
    OPTION,
    RESULT,
    FUNCTION,

    // Literals
    INTEGER_LITERAL,
    FLOATING_LITERAL,
    STRING_LITERAL,
    CHAR_LITERAL,
    BOOL_LITERAL,

    // Operators
    PLUS,
    MINUS,
    STAR,
    SLASH,
    PERCENT,
    EQUAL,
    EQUAL_EQUAL,
    BANG,
    BANG_EQUAL,
    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,
    AND,
    AND_AND,
    OR,
    OR_OR,
    ARROW,
    COLON,
    COLON_COLON,
    QUESTION,

    // Punctuation
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    COMMA,
    DOT,
    SEMICOLON,

    // Other
    IDENTIFIER,
    END_OF_FILE,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
};

#endif // CHTHOLLY_TOKEN_H
