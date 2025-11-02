#pragma once

#include "Token.h"
#include "AST/Expr.h"
#include "AST/Stmt.h"
#include <vector>
#include <stdexcept>

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::vector<std::unique_ptr<Stmt>> parse();

private:
    struct ParseError : std::runtime_error {
        using std::runtime_error::runtime_error;
    };

    std::vector<Token> tokens;
    int current = 0;

    std::unique_ptr<Stmt> declaration();
    std::unique_ptr<Stmt> letDeclaration();
    std::unique_ptr<Stmt> statement();
    std::unique_ptr<Stmt> printStatement();
    std::unique_ptr<Stmt> expressionStatement();

    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> equality();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> primary();

    bool match(const std::vector<TokenType>& types);
    Token advance();
    bool isAtEnd();
    Token peek();
    Token previous();
    Token consume(TokenType type, const std::string& message);
    ParseError error(const Token& token, const std::string& message);
    void synchronize();
};
