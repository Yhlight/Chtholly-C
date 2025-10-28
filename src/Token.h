#ifndef CHTHOLLY_TOKEN_H
#define CHTHOLLY_TOKEN_H

#include <string>
#include <variant>

namespace Chtholly
{

enum class TokenType
{
    // Meta
    EndOfFile,
    Unknown,

    // Literals
    Identifier,
    Integer,
    Double,
    String,
    Char,

    // Keywords
    Func, Let, Mut, Return, Struct, Public, Private,
    If, Else, Switch, Case, Fallthrough, For, While,
    Import,

    // Operators
    Plus, Minus, Star, Slash, Mod,
    Assign, Equal, NotEqual, Less, LessEqual, Greater, GreaterEqual,
    LogicalAnd, LogicalOr, LogicalNot,
    Ampersand, BitwiseOr,

    // Punctuation
    LParen, RParen, LBrace, RBrace, LBracket, RBracket,
    Semicolon, Colon, Comma, Arrow,
};

struct Token
{
    TokenType type;
    std::string text;
    std::variant<std::monostate, long long, double, std::string, char> value;
    int line = 0;
    int column = 0;

    bool operator==(const Token& other) const {
        return type == other.type && text == other.text;
    }
};

} // namespace Chtholly

#endif // CHTHOLLY_TOKEN_H
