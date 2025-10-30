#pragma once

#include <string>
#include <variant>

namespace chtholly {

enum class TokenType {
    // Punctuators
    LeftParen, RightParen,   // ( )
    LeftBrace, RightBrace,   // { }
    LeftBracket, RightBracket, // [ ]
    Comma, Dot, Semicolon, Colon, // , . ; :
    Arrow, FatArrow,      // -> =>

    // Operators
    Plus, Minus, Star, Slash, Mod, // + - * / %
    PlusPlus, MinusMinus,       // ++ --
    PlusEqual, MinusEqual, StarEqual, SlashEqual, ModEqual, // += -= *= /= %=
    Equal, EqualEqual, Bang, BangEqual,   // = == ! !=
    Less, LessEqual, Greater, GreaterEqual, // < <= > >=
    Amp, AmpAmp, Pipe, PipePipe,       // & && | ||
    Question,                   // ?

    // Keywords
    Func, Let, Mut,
    If, Else, Switch, Case, Fallthrough, Break,
    Return,
    Struct, Public, Private, Self, Impl, Trait,
    Import,

    // Literals
    Identifier,
    Integer,
    Double,
    String,
    Char,
    True, False, None,

    // Types (Some might be handled as identifiers, but good to have them)
    Int, UInt, Float, Void, Bool,

    // Misc
    Comment,
    EndOfFile
};

struct Token {
    TokenType type;
    std::string lexeme;
    std::variant<std::monostate, int, double, std::string, char> literal;
    int line;

    Token(TokenType type, std::string lexeme, std::variant<std::monostate, int, double, std::string, char> literal, int line)
        : type(type), lexeme(std::move(lexeme)), literal(std::move(literal)), line(line) {}
};

} // namespace chtholly
