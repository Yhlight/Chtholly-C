#ifndef CHTHOLLY_PARSER_H
#define CHTHOLLY_PARSER_H

#include "Token.h"
#include "AST.h"
#include <vector>
#include <memory>
#include <stdexcept>

class Parser {
public:
    Parser(std::vector<Token> tokens);
    std::vector<std::shared_ptr<Stmt>> parse();

private:
    std::vector<Token> tokens;
    int current = 0;

    std::shared_ptr<Stmt> declaration();
    std::shared_ptr<Stmt> structDeclaration();
    std::shared_ptr<Stmt> function(const std::string& kind, bool is_public = true);
    std::shared_ptr<Stmt> varDeclaration(bool isMutable, bool is_public, bool in_struct = false);
    std::shared_ptr<Stmt> statement();
    std::shared_ptr<Stmt> returnStatement();
    std::shared_ptr<Stmt> expressionStatement();
    std::vector<std::shared_ptr<Stmt>> block();

    std::shared_ptr<Expr> expression();
    std::shared_ptr<Expr> assignment();
    std::shared_ptr<Expr> equality();
    std::shared_ptr<Expr> comparison();
    std::shared_ptr<Expr> term();
    std::shared_ptr<Expr> factor();
    std::shared_ptr<Expr> unary();
    std::shared_ptr<Expr> call();
    std::shared_ptr<Expr> primary();
    std::shared_ptr<Expr> finishCall(std::shared_ptr<Expr> callee);

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
