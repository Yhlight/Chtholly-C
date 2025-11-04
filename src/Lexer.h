#pragma once

#include "Token.h"
#include <vector>
#include <string>

namespace chtholly {

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> scanTokens();

private:
    void scanToken();
    char advance();
    void addToken(TokenType type);
    void addToken(TokenType type, const std::variant<std::monostate, std::string, double, bool>& literal);
    bool match(char expected);
    char peek();
    void string();
    void number();
    void identifier();
    char peekNext();

    std::string source;
    std::vector<Token> tokens;
    int start = 0;
    int current = 0;
    int line = 1;
};

} // namespace chtholly
