#include "Parser.h"
#include <stdexcept>

namespace chtholly {

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

std::unique_ptr<BlockStmtAST> Parser::parse() {
    std::vector<std::unique_ptr<StmtAST>> statements;
    while (!isAtEnd()) {
        statements.push_back(parseStatement());
    }
    return std::make_unique<BlockStmtAST>(std::move(statements));
}

std::unique_ptr<StmtAST> Parser::parseStatement() {
    if (match({TokenType::Let, TokenType::Mut})) {
        return parseVarDecl();
    }
    // Other statements will be handled here
    return nullptr;
}

std::unique_ptr<StmtAST> Parser::parseVarDecl() {
    bool isMutable = peek(-1).type == TokenType::Mut;
    Token name = consume(TokenType::Identifier, "Expect variable name.");

    std::unique_ptr<ExprAST> initializer = nullptr;
    if (match({TokenType::Equal})) {
        initializer = parseExpression();
    }

    consume(TokenType::Semicolon, "Expect ';' after variable declaration.");
    return std::make_unique<VarDeclAST>(name.lexeme, std::move(initializer), isMutable);
}

std::unique_ptr<ExprAST> Parser::parseExpression() {
    return parseAssignment();
}

std::unique_ptr<ExprAST> Parser::parseAssignment() {
    auto expr = parseTerm();
    // Assignment logic will be here
    return expr;
}

std::unique_ptr<ExprAST> Parser::parseTerm() {
    auto expr = parseFactor();
    while (match({TokenType::Plus, TokenType::Minus})) {
        Token op = tokens[current - 1];
        auto right = parseFactor();
        expr = std::make_unique<BinaryExprAST>(op.type, std::move(expr), std::move(right));
    }
    return expr;
}

std::unique_ptr<ExprAST> Parser::parseFactor() {
    auto expr = parseUnary();
    while (match({TokenType::Star, TokenType::Slash})) {
        Token op = tokens[current - 1];
        auto right = parseUnary();
        expr = std::make_unique<BinaryExprAST>(op.type, std::move(expr), std::move(right));
    }
    return expr;
}

std::unique_ptr<ExprAST> Parser::parseUnary() {
    // Unary logic will be here
    return parsePrimary();
}

std::unique_ptr<ExprAST> Parser::parsePrimary() {
    if (match({TokenType::Integer})) {
        return std::make_unique<NumberExprAST>(std::get<int>(tokens[current - 1].literal));
    }
    if (match({TokenType::Double})) {
        return std::make_unique<NumberExprAST>(std::get<double>(tokens[current - 1].literal));
    }
    if (match({TokenType::Identifier})) {
        return std::make_unique<VariableExprAST>(tokens[current - 1].lexeme);
    }
    // Other primary expressions will be here
    return nullptr;
}

Token& Parser::peek(int k) {
    if (current + k >= tokens.size()) {
        return tokens.back();
    }
    return tokens[current + k];
}

Token& Parser::advance() {
    if (!isAtEnd()) current++;
    return tokens[current - 1];
}

bool Parser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (peek().type == type) {
            advance();
            return true;
        }
    }
    return false;
}

Token& Parser::consume(TokenType type, const std::string& message) {
    if (peek().type == type) return advance();
    throw std::runtime_error(message);
}

bool Parser::isAtEnd() {
    return peek().type == TokenType::EndOfFile;
}

} // namespace chtholly
