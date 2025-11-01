#pragma once

#include "Token.h"
#include <string>
#include <vector>

namespace chtholly {

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> scanTokens();

private:
    Token scanToken();
    bool isAtEnd() const;
    char advance();
    char peek() const;
    char peekNext() const;
    bool match(char expected);
    Token makeToken(TokenType type) const;
    Token makeToken(TokenType type, const Literal& literal) const;
    Token errorToken(const std::string& message) const;
    void skipWhitespace();
    Token string();
    Token number();
    Token identifier();
    TokenType identifierType();

    const std::string source;
    int start = 0;
    int current = 0;
    int line = 1;
};

} // namespace chtholly
