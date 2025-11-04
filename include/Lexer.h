#ifndef CHTHOLLY_LEXER_H
#define CHTHOLLY_LEXER_H

#include <string>
#include <vector>
#include <map>
#include "Token.h"

class Lexer {
public:
    explicit Lexer(std::string source);
    std::vector<Token> scanTokens();

private:
    std::string source;
    std::vector<Token> tokens;
    static const std::map<std::string, TokenType> keywords;

    int start = 0;
    int current = 0;
    int line = 1;

    bool isAtEnd();
    void scanToken();
    char advance();
    void addToken(TokenType type);
    bool match(char expected);
    char peek();
    void string();
    void number();
    void identifier();
    char peekNext();
};

#endif //CHTHOLLY_LEXER_H
