#pragma once

#include "AST.h"
#include "Token.h"
#include <vector>
#include <stdexcept>

namespace chtholly {

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::vector<std::shared_ptr<Stmt>> parse();

private:
    class ParseError : public std::runtime_error {
    public:
        ParseError() : std::runtime_error("") {}
    };

    std::shared_ptr<Stmt> declaration();
    std::shared_ptr<Stmt> letDeclaration();
    std::shared_ptr<Stmt> statement();
    std::shared_ptr<Stmt> ifStatement();
    std::shared_ptr<Stmt> whileStatement();
    std::shared_ptr<Stmt> expressionStatement();
    std::vector<std::shared_ptr<Stmt>> block();

    std::shared_ptr<Expr> expression();
    std::shared_ptr<Expr> assignment();
    std::shared_ptr<Expr> logical_or();
    std::shared_ptr<Expr> logical_and();
    std::shared_ptr<Expr> equality();
    std::shared_ptr<Expr> comparison();
    std::shared_ptr<Expr> term();
    std::shared_ptr<Expr> factor();
    std::shared_ptr<Expr> unary();
    std::shared_ptr<Expr> primary();

    bool match(const std::vector<TokenType>& types);
    Token consume(TokenType type, const std::string& message);
    bool check(TokenType type);
    Token advance();
    bool isAtEnd();
    Token peek();
    Token previous();

    ParseError error(const Token& token, const std::string& message);
    void synchronize();

    const std::vector<Token>& tokens;
    int current = 0;
};

} // namespace chtholly
