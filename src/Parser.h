#pragma once

#include "Token.h"
#include "AST.h"
#include <vector>
#include <memory>

namespace chtholly {

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::unique_ptr<Expr> parse();

private:
    std::unique_ptr<Expr> primary();
    bool match(const std::vector<TokenType>& types);
    bool check(TokenType type) const;
    const Token& peek() const;
    bool isAtEnd() const;
    const Token& advance();
    const Token& previous() const;

    const std::vector<Token>& tokens;
    size_t current = 0;
};

} // namespace chtholly
