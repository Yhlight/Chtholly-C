#ifndef CHTHOLLY_PARSER_H
#define CHTHOLLY_PARSER_H

#include <vector>
#include <stdexcept>
#include "Token.h"
#include "AST.h"

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);
    std::vector<std::shared_ptr<Stmt>> parse();

private:
    std::vector<Token> tokens;
    int current = 0;

    std::shared_ptr<Stmt> declaration();
    std::shared_ptr<Stmt> varDeclaration();
    std::shared_ptr<Stmt> statement();
    std::shared_ptr<Stmt> printStatement();
    std::shared_ptr<Stmt> ifStatement();
    std::shared_ptr<Stmt> expressionStatement();
    std::vector<std::shared_ptr<Stmt>> block();

    std::shared_ptr<Expr> expression();
    std::shared_ptr<Expr> assignment();
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

    // Error handling
    class ParseError : public std::runtime_error {
    public:
        ParseError() : std::runtime_error("") {}
    };
    ParseError error(const Token& token, const std::string& message);
    void synchronize();
};

#endif //CHTHOLLY_PARSER_H
