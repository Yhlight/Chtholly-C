#include "Token.h"

std::string to_string(Token token) {
    switch (token) {
        case Token::Eof: return "Eof";
        case Token::Func: return "Func";
        case Token::Let: return "Let";
        case Token::Mut: return "Mut";
        case Token::If: return "If";
        case Token::Else: return "Else";
        case Token::Struct: return "Struct";
        case Token::While: return "While";
        case Token::Import: return "Import";
        case Token::Impl: return "Impl";
        case Token::For: return "For";
        case Token::Trait: return "Trait";
        case Token::Identifier: return "Identifier";
        case Token::Number: return "Number";
        case Token::Operator: return "Operator";
        case Token::Semicolon: return "Semicolon";
        case Token::LeftParen: return "LeftParen";
        case Token::RightParen: return "RightParen";
        case Token::LeftBrace: return "LeftBrace";
        case Token::RightBrace: return "RightBrace";
        case Token::LeftBracket: return "LeftBracket";
        case Token::RightBracket: return "RightBracket";
        case Token::Comma: return "Comma";
        case Token::Colon: return "Colon";
        case Token::Unknown: return "Unknown";
        default: return "Unknown";
    }
}
