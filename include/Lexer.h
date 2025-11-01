#ifndef CHTHOLLY_LEXER_H
#define CHTHOLLY_LEXER_H

#include "Token.h"
#include <string>
#include <vector>
#include <unordered_map>

class Lexer {
public:
    Lexer(std::string source);
    std::vector<Token> scanTokens();

private:
    const std::string source;
    std::vector<Token> tokens;
    int start = 0;
    int current = 0;
    int line = 1;

    static const std::unordered_map<std::string, TokenType> keywords;

    bool isAtEnd();
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
};

#endif //CHTHOLLY_LEXER_H
