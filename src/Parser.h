#pragma once

#include "AST.h"
#include "Token.h"
#include <vector>
#include <stdexcept>

namespace chtholly {

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::vector<std::unique_ptr<Stmt>> parse();

private:
    struct ParseError : public std::runtime_error {
        using std::runtime_error::runtime_error;
    };

    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> equality();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> primary();

    std::unique_ptr<Stmt> declaration();
    std::unique_ptr<Stmt> varDeclaration();
    std::unique_ptr<Stmt> statement();
    std::unique_ptr<Stmt> expressionStatement();
    std::unique_ptr<TypeExpr> type();
    std::vector<std::unique_ptr<Stmt>> block();

    bool match(const std::vector<TokenType>& types);
    Token consume(TokenType type, const std::string& message);
    bool check(TokenType type);
    Token advance();
    bool isAtEnd();
    Token peek();
    Token previous();
    ParseError error(const Token& token, const std::string& message);
    void synchronize();

    std::vector<Token> tokens;
    int current = 0;
};

} // namespace chtholly
