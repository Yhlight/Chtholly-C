#pragma once

#include <string>

enum class Token {
    // End of file
    Eof,

    // Commands
    Func,
    Let,
    Mut,
    If,
    Else,
    Struct,
    While,
    Import,
    Impl,

    // Primary
    Identifier,
    Number,

    // Operators
    Operator,

    // Punctuation
    Semicolon,
    LeftParen,
    RightParen,
    LeftBrace,
    RightBrace,
    LeftBracket,
    RightBracket,
    Comma,
    Colon,

    // Unknown token
    Unknown
};

std::string to_string(Token token);
