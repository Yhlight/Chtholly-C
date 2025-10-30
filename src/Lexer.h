#ifndef CHTHOLLY_LEXER_H
#define CHTHOLLY_LEXER_H

#include "Token.h"
#include <string>
#include <vector>
#include <unordered_map>

class Lexer {
public:
    explicit Lexer(std::string source);
    Token nextToken();

private:
    char advance();
    char peek() const;
    bool isAtEnd() const;
    void skipWhitespace();
    Token makeToken(TokenType type) const;
    Token identifier();
    Token number();
    Token stringLiteral();
    Token charLiteral();


    std::string source_;
    size_t start_ = 0;
    size_t current_ = 0;
    int line_ = 1;
    int col_ = 1;

    static const std::unordered_map<std::string, TokenType> keywords;
};

#endif // CHTHOLLY_LEXER_H
