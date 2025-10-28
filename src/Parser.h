#ifndef CHTHOLLY_PARSER_H
#define CHTHOLLY_PARSER_H

#include "AST/AST.h"
#include "Token.h"
#include <vector>
#include <memory>

namespace Chtholly
{

class Parser
{
public:
    Parser(std::vector<Token> tokens);

    std::vector<std::unique_ptr<Stmt>> parse();
    bool hadError = false;

private:
    std::unique_ptr<Stmt> declaration();
    std::unique_ptr<Stmt> statement();
    std::unique_ptr<Stmt> varDeclaration();
    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> primary();

    bool match(const std::vector<TokenType>& types);
    Token consume(TokenType type, const std::string& message);
    void synchronize();
    bool check(TokenType type) const;
    Token advance();
    bool isAtEnd() const;
    Token peek() const;
    Token previous() const;

    std::vector<Token> tokens;
    int current = 0;
};

} // namespace Chtholly

#endif // CHTHOLLY_PARSER_H
