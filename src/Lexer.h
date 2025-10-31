#pragma once

#include "Token.h"
#include <string>

namespace chtholly {

class Lexer {
public:
    Lexer(const std::string& source);

    Token scanToken();

private:
    char advance();
    char peek() const;
    char peekNext() const;
    bool isAtEnd() const;
    void skipWhitespace();
    Token makeToken(TokenType type) const;
    Token errorToken(const std::string& message) const;

    std::string source;
    size_t start = 0;
    size_t current = 0;
    int line = 1;
};

} // namespace chtholly
