#include "Token.h"
#include <sstream>

Token::Token(TokenType type, std::string lexeme, std::any literal, int line)
    : type(type), lexeme(std::move(lexeme)), literal(std::move(literal)), line(line) {}

std::string Token::toString() const {
    std::stringstream ss;
    ss << "Token[Type=" << static_cast<int>(type) << ", Lexeme='" << lexeme << "', Line=" << line << "]";
    return ss.str();
}
