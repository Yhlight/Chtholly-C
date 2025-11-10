#ifndef CHTHOLLY_TOKEN_H
#define CHTHOLLY_TOKEN_H

#include <string>
#include <variant>
#include <utility>

namespace chtholly {

enum class TokenType {
    // Single-character tokens.
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE, LEFT_BRACKET, RIGHT_BRACKET,
    COMMA, DOT, MINUS, PLUS, SLASH, STAR, PERCENT, SEMICOLON, COLON, QUESTION,

    // One or two character tokens.
    STAR_STAR,
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,
    AMPERSAND, AND, // & and &&
    PIPE, OR,      // | and ||
    CARET, TILDE,   // ^ and ~
    LESS_LESS, GREATER_GREATER, // << and >>

    // Compound assignment operators
    PLUS_EQUAL, MINUS_EQUAL, STAR_EQUAL, SLASH_EQUAL, PERCENT_EQUAL,
    AMPERSAND_EQUAL, PIPE_EQUAL, CARET_EQUAL, // &=, |=, ^=
    LESS_LESS_EQUAL, GREATER_GREATER_EQUAL, // <<=, >>=

    // Other multi-character tokens
    ARROW, COLON_COLON, PLUS_PLUS, MINUS_MINUS,

    // Literals.
    IDENTIFIER, STRING_LITERAL, NUMBER_LITERAL, CHAR_LITERAL,

    // Keywords.
    IF, ELSE, FOR, WHILE, SWITCH, CASE, DEFAULT, BREAK, FALLTHROUGH,
    FUNC, RETURN, STRUCT, ENUM, IMPL, TRAIT, LET, MUT,
    PUBLIC, PRIVATE, SELF, NONE, TRUE, FALSE, IMPORT,

    // Built-in functions that are treated like keywords by the lexer
    PRINT, INPUT, TYPE_CAST,

    // Module names and their functions
    OS,
    TIME,
    IOSTREAM,
    FILESYSTEM, FS_READ, FS_WRITE,
    OPERATOR, REFLECT, UTIL,
    META,

    // Type Keywords
    INT, INT8, INT16, INT32, INT64,
    UINT,
    UINT8, UINT16, UINT32, UINT64,
    CHAR_TYPE, DOUBLE, FLOAT, LONG_DOUBLE,
    VOID, BOOL, STRING_TYPE, ARRAY, FUNCTION, OPTION, RESULT,

    // Special Tokens
    ERROR, END_OF_FILE
};

struct Token {
    TokenType type;
    std::string lexeme;
    std::variant<std::nullptr_t, std::string, double, long long, bool, char> literal;
    int line;

    Token(TokenType type, std::string lexeme,
          std::variant<std::nullptr_t, std::string, double, long long, bool, char> literal,
          int line)
        : type(type), lexeme(std::move(lexeme)), literal(std::move(literal)), line(line) {}
};

} // namespace chtholly

#endif // CHTHOLLY_TOKEN_H
