#ifndef CHTHOLLY_LEXER_H
#define CHTHOLLY_LEXER_H

#include "Token.h"
#include <string>

namespace Chtholly {

class Lexer {
public:
    Lexer(const std::string& source);
    Token nextToken();

private:
    std::string source;
    int position;
    int line;
    int column;

    char currentChar();
    void advance();
    void skipWhitespace();
    Token identifier();
    Token number();
    Token stringLiteral();
    Token charLiteral();
};

} // namespace Chtholly

#endif // CHTHOLLY_LEXER_H
