#pragma once

#include "Token.h"
#include <vector>
#include <string>
#include <map>

class Lexer {
public:
    Lexer(std::string source);
    std::vector<Token> scanTokens();

private:
    std::string source;
    std::vector<Token> tokens;
    int start = 0;
    int current = 0;
    int line = 1;

    static const std::map<std::string, TokenType> keywords;

    bool isAtEnd();
    void scanToken();
    char advance();
    void addToken(TokenType type);
    void addToken(TokenType type, const std::any& literal);
    bool match(char expected);
    char peek();
    char peekNext();
    void string();
    void number();
    void identifier();
};
