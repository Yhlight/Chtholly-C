#pragma once

#include "Token.h"
#include <string>
#include <vector>

namespace chtholly {

class Lexer {
public:
    Lexer(std::string source);
    std::vector<Token> scanTokens();

private:
    void scanToken();
    char advance();
    void addToken(TokenType type);
    void addToken(TokenType type, const std::variant<std::monostate, int, double, std::string, char>& literal);
    bool match(char expected);
    char peek();
    char peekNext();
    void string();
    void number();
    void identifier();
    void blockComment();

    std::string source;
    std::vector<Token> tokens;
    int start = 0;
    int current = 0;
    int line = 1;
};

} // namespace chtholly
