#ifndef CHTHOLLY_TOKEN_H
#define CHTHOLLY_TOKEN_H

#include "TokenType.h"
#include <string>
#include <utility>
#include <any>

class Token {
public:
    Token(TokenType type, std::string lexeme, std::any literal, int line)
        : type(type), lexeme(std::move(lexeme)), literal(std::move(literal)), line(line) {}

    std::string toString() const {
        return "Type: " + std::to_string(static_cast<int>(type)) + ", Lexeme: " + lexeme;
    }

    const TokenType type;
    const std::string lexeme;
    const std::any literal;
    const int line;
};

#endif //CHTHOLLY_TOKEN_H
