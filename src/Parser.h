#pragma once

#include "Token.h"
#include "AST/Expr.h"
#include "AST/Stmt.h"
#include <vector>
#include <stdexcept>

class PrattParser;

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    ~Parser();
    std::vector<std::unique_ptr<Stmt>> parse();

private:
    struct ParseError : std::runtime_error {
        using std::runtime_error::runtime_error;
    };

    friend class PrattParser;
    std::vector<Token> tokens;
    int current = 0;
    std::unique_ptr<PrattParser> prattParser;

    std::unique_ptr<Stmt> declaration();
    std::unique_ptr<Stmt> structDeclaration();
    std::unique_ptr<Stmt> traitDeclaration();
    std::unique_ptr<Stmt> implDeclaration();
    std::unique_ptr<Stmt> importDeclaration();
    std::unique_ptr<Stmt> function(const std::string& kind, bool body_required);
    std::unique_ptr<Stmt> letDeclaration();
    std::unique_ptr<Stmt> statement();
    std::unique_ptr<Stmt> printStatement();
    std::unique_ptr<Stmt> returnStatement();
    std::unique_ptr<Stmt> expressionStatement();
    std::unique_ptr<Stmt> ifStatement();
    std::unique_ptr<Stmt> whileStatement();
    std::vector<std::unique_ptr<Stmt>> block();

    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> assignment();

    TypeInfo parseType();
    bool LA_is_generic_call();
    bool match(const std::vector<TokenType>& types);
    Token advance();
    bool isAtEnd();
    bool check(TokenType type);
    Token peek();
    Token previous();
    Token consume(TokenType type, const std::string& message);
    ParseError error(const Token& token, const std::string& message);
    void synchronize();
};
