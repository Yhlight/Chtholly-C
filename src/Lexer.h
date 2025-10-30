#ifndef CHTHOLLY_LEXER_H
#define CHTHOLLY_LEXER_H

#include "Token.h"
#include <string>

namespace Chtholly {

class Lexer {
public:
    Lexer(const std::string& input);

    Token nextToken();

private:
    void readChar();
    char peekChar();
    void skipWhitespace();
    bool isLetter(char ch);
    bool isDigit(char ch);
    std::string readIdentifier();
    std::string readNumber();
    std::string readString();
    std::string readCharLiteral();

    static const std::unordered_map<std::string, TokenType> keywords;

    std::string m_input;
    int m_position;      // current position in input (points to current char)
    int m_readPosition;  // current reading position in input (after current char)
    char m_ch;           // current char under examination
    int m_line;
    int m_col;
};

} // namespace Chtholly

#endif // CHTHOLLY_LEXER_H
