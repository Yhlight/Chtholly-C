#ifndef CHTHOLLY_TOKEN_H
#define CHTHOLLY_TOKEN_H

#include "TokenType.h"
#include <string>
#include <utility>

class Token {
public:
    const TokenType type;
    const std::string lexeme;
    const int line;

    Token(TokenType type, std::string lexeme, int line)
        : type(type), lexeme(std::move(lexeme)), line(line) {}

    std::string toString() const;
};

// It's often better to have a separate function to convert enum to string
// for better separation of concerns and to avoid cluttering the header.
// I will create a helper function for this later if needed.

#endif //CHTHOLLY_TOKEN_H
