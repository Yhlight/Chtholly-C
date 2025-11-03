#ifndef CHTHOLLY_LEXER_H
#define CHTHOLLY_LEXER_H

#include "Token.h"
#include <vector>
#include <string>

class Lexer {
public:
    explicit Lexer(std::string source);
    std::vector<Token> scanTokens();

private:
    void scanToken();
    char advance();
    void addToken(TokenType type);
    void addToken(TokenType type, const std::any& literal);
    bool match(char expected);
    char peek();
    char peekNext();
    void string();
    void number();
    void identifier();
    bool isAtEnd() const;

    std::string source;
    std::vector<Token> tokens;
    int start = 0;
    int current = 0;
    int line = 1;
};

#endif //CHTHOLLY_LEXER_H
