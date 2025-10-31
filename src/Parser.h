#pragma once

#include "Token.h"
#include "AST.h"
#include <vector>
#include <memory>

namespace chtholly {

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::vector<std::unique_ptr<Stmt>> parse();

private:
    std::unique_ptr<Stmt> declaration();
    std::unique_ptr<Stmt> statement();
    std::unique_ptr<Stmt> varDeclaration();
    std::unique_ptr<Stmt> ifStatement();
    std::vector<std::unique_ptr<Stmt>> block();
    std::unique_ptr<Stmt> expressionStatement();
    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> primary();

    bool match(const std::vector<TokenType>& types);
    bool check(TokenType type) const;
    const Token& peek() const;
    bool isAtEnd() const;
    const Token& advance();
    const Token& previous() const;
    const Token& consume(TokenType type, const std::string& message);
    ParseError error(const Token& token, const std::string& message) const;
    void synchronize();

    const std::vector<Token>& tokens;
    size_t current = 0;
};

} // namespace chtholly
