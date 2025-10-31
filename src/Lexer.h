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
    bool match(char expected);
    void skipWhitespace();
    Token makeToken(TokenType type) const;
    Token errorToken(const std::string& message) const;
    bool isDigit(char c) const;
    bool isAlpha(char c) const;
    bool isAlphaNumeric(char c) const;
    Token number();
    Token string();
    Token identifier();

    std::string source;
    size_t start = 0;
    size_t current = 0;
    int line = 1;
};

} // namespace chtholly
