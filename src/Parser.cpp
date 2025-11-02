#include "Parser.h"
#include "Error.h"

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

std::vector<std::unique_ptr<Stmt>> Parser::parse() {
    std::vector<std::unique_ptr<Stmt>> statements;
    while (!isAtEnd()) {
        statements.push_back(declaration());
    }
    return statements;
}

std::unique_ptr<Stmt> Parser::declaration() {
    try {
        if (match({TokenType::LET})) return letDeclaration();
        return statement();
    } catch (ParseError& error) {
        synchronize();
        return nullptr;
    }
}

std::unique_ptr<Stmt> Parser::letDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");
    std::unique_ptr<Expr> initializer = nullptr;
    if (match({TokenType::EQUAL})) {
        initializer = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_unique<LetStmt>(name, std::move(initializer));
}

std::unique_ptr<Stmt> Parser::statement() {
    if (match({TokenType::PRINT})) return printStatement();
    return expressionStatement();
}

std::unique_ptr<Stmt> Parser::printStatement() {
    auto value = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after value.");
    return std::make_unique<PrintStmt>(std::move(value));
}

std::unique_ptr<Stmt> Parser::expressionStatement() {
    auto expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    return std::make_unique<ExpressionStmt>(std::move(expr));
}

std::unique_ptr<Expr> Parser::expression() {
    return equality();
}

std::unique_ptr<Expr> Parser::equality() {
    auto expr = comparison();

    while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
        Token op = previous();
        auto right = comparison();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::comparison() {
    auto expr = term();

    while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL})) {
        Token op = previous();
        auto right = term();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::term() {
    auto expr = factor();

    while (match({TokenType::MINUS, TokenType::PLUS})) {
        Token op = previous();
        auto right = factor();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::factor() {
    auto expr = unary();

    while (match({TokenType::SLASH, TokenType::STAR})) {
        Token op = previous();
        auto right = unary();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::unary() {
    if (match({TokenType::BANG, TokenType::MINUS})) {
        Token op = previous();
        auto right = unary();
        return std::make_unique<UnaryExpr>(op, std::move(right));
    }

    return primary();
}

std::unique_ptr<Expr> Parser::primary() {
    if (match({TokenType::FALSE})) return std::make_unique<LiteralExpr>(false);
    if (match({TokenType::TRUE})) return std::make_unique<LiteralExpr>(true);

    if (match({TokenType::NUMBER, TokenType::STRING})) {
        return std::make_unique<LiteralExpr>(previous().literal);
    }

    if (match({TokenType::LEFT_PAREN})) {
        auto expr = expression();
        if (peek().type != TokenType::RIGHT_PAREN) {
            ErrorReporter::error(peek().line, "Expect ')' after expression.");
        }
        advance();
        return std::make_unique<GroupingExpr>(std::move(expr));
    }

    ErrorReporter::error(peek().line, "Expect expression.");
    throw std::runtime_error("Expect expression.");
}

bool Parser::match(const std::vector<TokenType>& types) {
    for (auto type : types) {
        if (!isAtEnd() && peek().type == type) {
            advance();
            return true;
        }
    }
    return false;
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::isAtEnd() {
    return peek().type == TokenType::END_OF_FILE;
}

Token Parser::peek() {
    return tokens[current];
}

Token Parser::previous() {
    return tokens[current - 1];
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (!isAtEnd() && peek().type == type) return advance();
    throw error(peek(), message);
}

Parser::ParseError Parser::error(const Token& token, const std::string& message) {
    ErrorReporter::error(token.line, message);
    return ParseError(message);
}

void Parser::synchronize() {
    advance();

    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return;

        switch (peek().type) {
            case TokenType::FUNC:
            case TokenType::LET:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::PRINT:
            case TokenType::RETURN:
                return;
        }

        advance();
    }
}
