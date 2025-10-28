#ifndef CHTHOLLY_PARSER_H
#define CHTHOLLY_PARSER_H

#include "AST/AST.h"
#include "Token.h"
#include <vector>
#include <memory>

namespace Chtholly
{

enum Precedence {
    PREC_NONE,
    PREC_ASSIGNMENT,  // =
    PREC_OR,          // or
    PREC_AND,         // and
    PREC_EQUALITY,    // == !=
    PREC_COMPARISON,  // < > <= >=
    PREC_TERM,        // + -
    PREC_FACTOR,      // * /
    PREC_UNARY,       // ! -
    PREC_CALL,        // . ()
    PREC_PRIMARY
};

class Parser
{
public:
    Parser(std::vector<Token> tokens);

    std::vector<std::unique_ptr<Stmt>> parse();
    bool hadError = false;

    // Public for parsing functions
    Token advance();
    Token previous() const;
    Token consume(TokenType type, const std::string& message);
    std::unique_ptr<Expr> parsePrecedence(Precedence precedence);
    std::unique_ptr<Expr> expression();


private:
    std::unique_ptr<Stmt> declaration();
    std::unique_ptr<Stmt> statement();
    std::unique_ptr<Stmt> varDeclaration();
    std::unique_ptr<Stmt> ifStatement();
    std::vector<std::unique_ptr<Stmt>> block();
    std::unique_ptr<Expr> assignment();
    std::unique_ptr<Expr> primary();

    bool match(const std::vector<TokenType>& types);
    void synchronize();
    bool check(TokenType type) const;
    bool isAtEnd() const;
    Token peek() const;

    std::vector<Token> tokens;
    int current = 0;
};

} // namespace Chtholly

#endif // CHTHOLLY_PARSER_H
