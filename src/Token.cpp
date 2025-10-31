#include "Token.h"

Token::Token(TokenType type, const std::string& lexeme, int line)
    : type(type), lexeme(lexeme), line(line) {}

std::string Token::toString() const {
    return "Token[type=" + std::to_string(static_cast<int>(type)) +
           ", lexeme='" + lexeme + "', line=" + std::to_string(line) + "]";
}
