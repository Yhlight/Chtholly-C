#ifndef CHTHOLLY_LEXER_H
#define CHTHOLLY_LEXER_H

#include <string>
#include <vector>
#include "Token.h"

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> scanTokens();

private:
    void scanToken();
    char advance();
    void addToken(TokenType type);
    void addToken(TokenType type, const std::string& literal);
    bool match(char expected);
    char peek();
    char peekNext();
    void string();
    void number();
    void identifier();

    std::string source_;
    std::vector<Token> tokens_;
    int start_ = 0;
    int current_ = 0;
    int line_ = 1;
};

#endif //CHTHOLLY_LEXER_H
