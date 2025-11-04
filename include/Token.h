#ifndef CHTHOLLY_TOKEN_H
#define CHTHOLLY_TOKEN_H

#include <string>
#include <utility>
#include "TokenType.h"

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    // std::any literal; // For later use

    Token(TokenType type, std::string lexeme, int line)
        : type(type), lexeme(std::move(lexeme)), line(line) {}
};

#endif //CHTHOLLY_TOKEN_H
