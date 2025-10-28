#include "Parser.h"
#include "Token.h"
#include "AST/AST.h"
#include <stdexcept>
#include <iostream>

namespace Chtholly
{

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

std::vector<std::unique_ptr<Stmt>> Parser::parse()
{
    std::vector<std::unique_ptr<Stmt>> statements;
    while (!isAtEnd())
    {
        statements.push_back(declaration());
    }
    return statements;
}

void Parser::synchronize() {
    advance();

    while (!isAtEnd()) {
        if (previous().type == TokenType::Semicolon) return;

        switch (peek().type) {
            case TokenType::Func:
            case TokenType::Let:
            case TokenType::For:
            case TokenType::If:
            case TokenType::While:
            case TokenType::Return:
                return;
        }

        advance();
    }
}

std::unique_ptr<Stmt> Parser::declaration()
{
    try {
        if (match({TokenType::Let})) return varDeclaration();
        return statement();
    } catch (std::runtime_error& error) {
        synchronize();
        hadError = true;
        std::cerr << error.what() << std::endl;
        return nullptr;
    }
}

std::unique_ptr<Stmt> Parser::statement()
{
    // For now, only variable declarations are statements
    throw std::runtime_error("Unexpected token: " + peek().text);
}

std::unique_ptr<Stmt> Parser::varDeclaration()
{
    Token name = consume(TokenType::Identifier, "Expect variable name.");
    std::unique_ptr<Expr> initializer = nullptr;
    if (match({TokenType::Assign}))
    {
        initializer = expression();
    }
    consume(TokenType::Semicolon, "Expect ';' after variable declaration.");
    return std::make_unique<VarDeclStmt>(name, std::move(initializer));
}

std::unique_ptr<Expr> Parser::expression()
{
    return primary();
}

std::unique_ptr<Expr> Parser::primary()
{
    if (match({TokenType::Integer, TokenType::Double}))
    {
        return std::make_unique<NumberLiteralExpr>(previous());
    }
    throw std::runtime_error("Expect expression.");
}

bool Parser::match(const std::vector<TokenType>& types)
{
    for (TokenType type : types)
    {
        if (check(type))
        {
            advance();
            return true;
        }
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& message)
{
    if (check(type)) return advance();
    throw std::runtime_error(message);
}

bool Parser::check(TokenType type) const
{
    if (isAtEnd()) return false;
    return peek().type == type;
}

Token Parser::advance()
{
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::isAtEnd() const
{
    return peek().type == TokenType::EndOfFile;
}

Token Parser::peek() const
{
    return tokens.at(current);
}

Token Parser::previous() const
{
    return tokens.at(current - 1);
}

} // namespace Chtholly
