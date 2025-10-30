#ifndef CHTHOLLY_TOKEN_H
#define CHTHOLLY_TOKEN_H

#include <string>
#include <unordered_map>

namespace Chtholly {

enum class TokenType {
    // Special Tokens
    Illegal,
    Eof,

    // Identifiers & Literals
    Identifier,
    Int,
    Double,
    String,
    Char,

    // Operators
    Assign,
    Plus,
    Minus,
    Bang,
    Asterisk,
    Slash,
    Mod,

    LessThan,
    GreaterThan,

    Equal,
    NotEqual,
    LessThanEqual,
    GreaterThanEqual,
    And,
    Or,

    // Delimiters
    Comma,
    Semicolon,
    Colon,
    LParen,
    RParen,
    LBrace,
    RBrace,
    LBracket,
    RBracket,

    // Keywords
    Func,
    Let,
    Mut,
    If,
    Else,
    Return,
    True,
    False,
    Struct,
    Import,
    Public,
    Private,
    Self,
    Switch,
    Case,
    Fallthrough,

    // Misc
    Arrow, // ->
};

struct Token {
    TokenType type;
    std::string literal;
    int line;
    int col;
};

} // namespace Chtholly

#endif // CHTHOLLY_TOKEN_H
