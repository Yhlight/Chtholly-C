#ifndef CHTHOLLY_PARSER_H
#define CHTHOLLY_PARSER_H

#include "Token.h"
#include "AST.h"
#include <vector>
#include <memory>
#include <stdexcept>

namespace chtholly {

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);
    std::vector<std::unique_ptr<Stmt>> parse();

private:
    struct ParseError : public std::runtime_error {
        using std::runtime_error::runtime_error;
    };

    std::vector<Token> tokens;
    int current = 0;

    // Grammar rules
    std::unique_ptr<Stmt> declaration();
    std::unique_ptr<Stmt> varDeclaration();
    std::unique_ptr<Stmt> statement();
    std::unique_ptr<Stmt> expressionStatement();
    std::unique_ptr<Stmt> ifStatement();
    std::unique_ptr<Stmt> whileStatement();
    std::unique_ptr<Stmt> switchStatement();
    std::unique_ptr<CaseStmt> caseStatement();
    std::unique_ptr<Stmt> returnStatement();
    std::unique_ptr<Stmt> importStatement();
    std::unique_ptr<FunctionStmt> functionDeclaration(const std::string& kind);
    std::unique_ptr<Stmt> structDeclaration();
    std::vector<std::unique_ptr<Stmt>> block();

    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> assignment();
    std::unique_ptr<Expr> equality();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> call();
    std::unique_ptr<Expr> primary();

    // Helper methods
    bool isAtEnd();
    Token peek();
    Token previous();
    Token advance();
    bool check(TokenType type);
    template<typename... Args>
    bool match(Args... types);
    Token consume(TokenType type, const std::string& message);
    ParseError error(const Token& token, const std::string& message);
    void synchronize();
};

} // namespace chtholly

#endif // CHTHOLLY_PARSER_H
