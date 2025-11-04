#ifndef CHTHOLLY_LEXER_H
#define CHTHOLLY_LEXER_H

#include "Token.h"
#include <string>

class Lexer {
public:
    Lexer(const std::string& source);
    Token nextToken();

private:
    std::string source;
    size_t start = 0;
    size_t current = 0;
    int line = 1;
    int column = 1;

    char advance();
    bool isAtEnd();
    Token makeToken(TokenType type);
    Token errorToken(const std::string& message);
    void skipWhitespace();
    char peek();
    char peekNext();
    bool match(char expected);
    Token string();
    Token number();
    Token identifier();
    TokenType identifierType();
    TokenType checkKeyword(size_t start, size_t length, const char* rest, TokenType type);
};

#endif // CHTHOLLY_LEXER_H
