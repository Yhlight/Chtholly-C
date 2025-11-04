#pragma once

#include "AST/Expr.h"
#include "AST/Stmt.h"
#include "Token.h"
#include <vector>
#include <memory>

class BlockStmt;

class PrattParser {
public:
    PrattParser(const std::vector<Token>& tokens);
    std::vector<std::unique_ptr<Stmt>> parse();

private:
    std::unique_ptr<Stmt> declaration();
    std::unique_ptr<Stmt> letDeclaration();
    std::unique_ptr<Stmt> statement();
    std::unique_ptr<Stmt> ifStatement();
    std::unique_ptr<Stmt> whileStatement();
    std::unique_ptr<Stmt> function(const std::string& kind, bool body_required = true);
    std::unique_ptr<Stmt> structDeclaration();
    std::unique_ptr<Stmt> returnStatement();
    std::unique_ptr<Stmt> traitDeclaration();
    std::unique_ptr<Stmt> implDeclaration();
    std::unique_ptr<Stmt> importStatement();
    std::unique_ptr<Stmt> enumDeclaration();
    std::vector<std::unique_ptr<Stmt>> block();
    std::unique_ptr<Stmt> expressionStatement();

    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> prefix();
    std::unique_ptr<Expr> infix(std::unique_ptr<Expr> left);

    int precedence();
    const Token& peek() const;
    const Token& previous() const;
    bool isAtEnd() const;
    Token advance();
    bool check(TokenType type) const;
    template<typename... Types>
    bool match(Types... types);
    Token consume(TokenType type, const std::string& message);
    void synchronize();
    TypeInfo parseType();

    const std::vector<Token>& tokens;
    int current = 0;
};
