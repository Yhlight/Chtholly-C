#ifndef CHTHOLLY_LEXER_H
#define CHTHOLLY_LEXER_H

#include "Token.h"
#include <string>
#include <vector>

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> scanTokens();

private:
    std::string source;
    std::vector<Token> tokens;
    int start = 0;
    int current = 0;
    int line = 1;

    bool isAtEnd();
    char advance();
    void addToken(TokenType type);
    void addToken(TokenType type, const std::string& literal);
    void scanToken();
    char peek();
    char peekNext();
    bool match(char expected);
    void string();
    void number();
    void identifier();
    void singleLineComment();
    void multiLineComment();
};

#endif //CHTHOLLY_LEXER_H
