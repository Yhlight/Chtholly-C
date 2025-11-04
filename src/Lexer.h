#ifndef CHTHOLLY_LEXER_H
#define CHTHOLLY_LEXER_H

#include "Token.h"
#include <string>
#include <vector>
#include <map>

namespace chtholly {

class Lexer {
public:
    explicit Lexer(std::string source);
    std::vector<Token> scanTokens();

private:
    std::string source;
    std::vector<Token> tokens;
    int start = 0;
    int current = 0;
    int line = 1;

    static const std::map<std::string, TokenType> keywords;

    bool isAtEnd();
    void scanToken();
    char advance();
    void addToken(TokenType type);
    void addToken(TokenType type, const std::variant<std::nullptr_t, std::string, double, long long, bool, char>& literal);
    bool match(char expected);
    char peek();
    char peekNext();

    void string();
    void number();
    void identifier();
    void blockComment();
};

} // namespace chtholly

#endif // CHTHOLLY_LEXER_H
