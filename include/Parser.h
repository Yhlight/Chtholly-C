#ifndef CHTHOLLY_PARSER_H
#define CHTHOLLY_PARSER_H

#include "Token.h"
#include "AST.h"
#include <vector>
#include <memory>
#include <stdexcept>

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::vector<std::unique_ptr<Stmt>> parse();

private:
    std::vector<Token> tokens;
    size_t current = 0;
    bool hadError = false;

    std::unique_ptr<Stmt> declaration();
    std::unique_ptr<Stmt> function();
    std::unique_ptr<Stmt> varDeclaration();
    std::unique_ptr<Stmt> statement();
    std::unique_ptr<Stmt> ifStatement();
    std::unique_ptr<Stmt> whileStatement();
    std::unique_ptr<Stmt> returnStatement();
    std::unique_ptr<Stmt> expressionStatement();
    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> assignment();
    std::unique_ptr<Expr> logicalOr();
    std::unique_ptr<Expr> logicalAnd();
    std::unique_ptr<Expr> equality();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> call();
    std::unique_ptr<Expr> primary();

    bool match(const std::vector<TokenType>& types);
    Token consume(TokenType type, const std::string& message);
    bool check(TokenType type);
    Token advance();
    bool isAtEnd();
    Token peek();
    Token previous();
    void synchronize();

    class ParseError : public std::runtime_error {
    public:
        ParseError() : std::runtime_error("") {}
    };

    void error(const Token& token, const std::string& message);
    ParseError reportError(const Token& token, const std::string& message);
};

#endif // CHTHOLLY_PARSER_H
