#pragma once

#include "AST.h"
#include "Token.h"
#include <vector>
#include <initializer_list>
#include <stdexcept>


namespace chtholly {
class ParseError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};


class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::vector<std::unique_ptr<Stmt>> parse();

private:
    std::unique_ptr<Stmt> declaration();
    std::unique_ptr<Stmt> varDeclaration();
    std::unique_ptr<Stmt> statement();
    std::unique_ptr<Stmt> expressionStatement();

    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> equality();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> primary();

    bool match(std::initializer_list<TokenType> types);
    bool check(TokenType type) const;
    Token advance();
    bool isAtEnd() const;
    Token peek() const;
    Token previous() const;
    Token consume(TokenType type, const std::string& message);
    ParseError error(const Token& token, const std::string& message) const;
    void synchronize();

    const std::vector<Token>& tokens;
    int current = 0;
};

} // namespace chtholly
