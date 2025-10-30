#ifndef CHTHOLLY_PARSER_H
#define CHTHOLLY_PARSER_H

#include "Token.h"
#include "Lexer.h"
#include "AST/Stmt.h"
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
    std::unique_ptr<Stmt> letDeclaration();
    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> primary();

    Lexer& lexer_;
    std::vector<Token> tokens_;
    int current_ = 0;
};

#endif // CHTHOLLY_PARSER_H
