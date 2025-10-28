#pragma once

#include "Token.h"
#include <string>

class Lexer {
public:
    Lexer(std::string source);

    Token get_token();
    std::string get_identifier() const { return identifier_; }
    double get_number() const { return number_; }
    char get_operator() const { return operator_; }

private:
    char get_char();

    std::string source_;
    size_t position_ = 0;
    std::string identifier_;
    double number_ = 0.0;
    char operator_ = 0;
};
