#pragma once

#include "Token.h"
#include <vector>
#include <string>

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> scanTokens();

private:
    std::string source;
    std::vector<Token> tokens;
    int start = 0;
    int current = 0;
    int line = 1;

    bool isAtEnd();
    void scanToken();
    char advance();
    void addToken(TokenType type);
    void addToken(TokenType type, const LiteralValue& literal);
    bool match(char expected);
    char peek();
    void string();
    void number();
    void identifier();
    char peekNext();
};
