#pragma once

#include <string>
#include <utility>

enum class TokenType {
    // Keywords
    Let,
    Mut,
    Func,
    Struct,
    Enum,
    If,
    Else,
    Switch,
    SwitchCase,
    Return,
    True,
    False,
    None,

    // Literals
    Identifier,
    Integer,
    Float,
    StringLiteral,
    CharLiteral,

    // Operators
    Plus,
    Minus,
    Asterisk,
    Slash,
    Percent,
    Equal,
    DoubleEqual,
    NotEqual,
    LessThan,
    GreaterThan,
    LessThanOrEqual,
    GreaterThanOrEqual,
    LogicalAnd,
    LogicalOr,
    LogicalNot,
    BitwiseAnd,
    BitwiseOr,
    BitwiseXor,
    BitwiseNot,
    LeftShift,
    RightShift,

    // Punctuation
    LParen,
    RParen,
    LBrace,
    RBrace,
    LBracket,
    RBracket,
    Comma,
    Colon,
    Semicolon,
    Period,

    // Other
    Eof,
    Illegal
};

struct Token {
    TokenType type;
    std::string literal;
};
