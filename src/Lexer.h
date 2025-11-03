#pragma once

#include "Token.h"
#include <string>
#include <vector>
#include <map>

namespace chtholly {

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> scanTokens();

private:
    void scanToken();
    char advance();
    bool match(char expected);
    char peek();
    char peekNext();
    void string();
    void number();
    void identifier();

    const std::string source;
    std::vector<Token> tokens;
    int start = 0;
    int current = 0;
    int line = 1;

    static const std::map<std::string, TokenType> keywords;
};

} // namespace chtholly
