#pragma once

#include <string>

enum class Token {
    // End of file
    Eof,

    // Commands
    Func,
    Let,
    Mut,

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
    Comma,

    // Unknown token
    Unknown
};

std::string to_string(Token token);
