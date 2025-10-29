#ifndef CHTHOLLY_TOKEN_H
#define CHTHOLLY_TOKEN_H

#include <string>

namespace Chtholly {

enum class TokenType {
    // Keywords
    Func, Let, Mut, If, Else, Switch, Case, Return, Struct, Import, Public, Private,
    Fallthrough, Trait, Impl, While,

    // Identifiers and literals
    Identifier, Integer, Double, Char, String,

    // Operators
    Assign, Plus, Minus, Asterisk, Slash, Percent,
    Bang, Less, Greater,
    Equal, NotEqual, LessEqual, GreaterEqual,
    And, Or,

    // Punctuation
    LParen, RParen, LBrace, RBrace, LBracket, RBracket,
    Semicolon, Colon, Comma, Dot, Arrow, Question,

    // End of File
    Eof,

    // Unknown
    Unknown
};

struct Token {
    TokenType type;
    std::string literal;
    int line;
    int column;
};

} // namespace Chtholly

#endif // CHTHOLLY_TOKEN_H
