#pragma once

#include <string>
#include <vector>
#include "Token.h"

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
    char advance();
    void addToken(TokenType type);
    void addToken(TokenType type, const std::variant<std::monostate, std::string, double, bool>& literal);
    bool match(char expected);
    void scanToken();
    void string();
    void number();
    void identifier();
};
