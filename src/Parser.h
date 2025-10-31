#ifndef CHTHOLLY_PARSER_H
#define CHTHOLLY_PARSER_H

#include "Token.h"
#include "Lexer.h"
#include "AST/Stmt.h"
#include "Type.h"
#include <vector>
#include <memory>

class Parser {
public:
    explicit Parser(Lexer& lexer);
    std::vector<std::unique_ptr<Stmt>> parse();

private:
    Token advance();
    Token peek(int k = 0) const;
    bool isAtEnd() const;
    bool match(const std::vector<TokenType>& types);
    Token consume(TokenType type, const std::string& message);

    std::unique_ptr<Stmt> declaration();
    std::unique_ptr<Stmt> statement();
    std::unique_ptr<Stmt> letDeclaration(bool isMutable);
    std::unique_ptr<Stmt> functionDeclaration();
    std::unique_ptr<BlockStmt> block();
    std::unique_ptr<Stmt> returnStatement();
    std::unique_ptr<Expr> expression();
    std::unique_ptr<Type> parseType();
    std::unique_ptr<Stmt> structDeclaration();
    std::unique_ptr<Expr> structInitializer();
    std::unique_ptr<Expr> parsePrecedence(int precedence);
    std::unique_ptr<Expr> prefix();
    std::unique_ptr<Expr> infix(std::unique_ptr<Expr> left);
    int getPrecedence(TokenType type);

    Lexer& lexer_;
    std::vector<Token> tokens_;
    int current_ = 0;
};

#endif // CHTHOLLY_PARSER_H
