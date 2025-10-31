#pragma once

#include "Token.h"
#include <string>

class Lexer {
public:
    explicit Lexer(std::string source);
    Token next_token();

private:
    void skip_whitespace();
    void skip_comment();
    Token identifier();
    Token number();
    Token string_literal();
    Token char_literal();
    char peek();
    char advance();

    std::string source;
    size_t position = 0;
};
