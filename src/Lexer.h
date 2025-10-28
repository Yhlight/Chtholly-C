#ifndef CHTHOLLY_LEXER_H
#define CHTHOLLY_LEXER_H

#include "Token.h"
#include <string>
#include <vector>

namespace Chtholly
{

class Lexer
{
public:
    Lexer(const std::string& source);

    std::vector<Token> tokenize();

private:
    Token nextToken();

    char advance();
    char peek() const;
    char peekNext() const;
    bool isAtEnd() const;
    bool match(char expected);

    Token makeToken(TokenType type) const;
    void skipWhitespace();
    Token identifier();
    Token number();
    Token stringLiteral();
    Token charLiteral();

    std::string source;
    size_t start = 0;
    size_t current = 0;
    int line = 1;
};

} // namespace Chtholly

#endif // CHTHOLLY_LEXER_H
