#include "Parser.h"

namespace chtholly {

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

std::vector<std::unique_ptr<Stmt>> Parser::parse() {
    std::vector<std::unique_ptr<Stmt>> statements;
    while (!isAtEnd()) {
        try {
            statements.push_back(declaration());
        } catch (ParseError& error) {
            synchronize();
        }
    }
    return statements;
}

std::unique_ptr<Expr> Parser::expression() {
    return equality();
}

std::unique_ptr<Expr> Parser::call() {
    auto expr = primary();
    while (true) {
        if (match({TokenType::LEFT_PAREN})) {
            expr = finishCall(std::move(expr));
        } else {
            break;
        }
    }
    return expr;
}

std::unique_ptr<Expr> Parser::finishCall(std::unique_ptr<Expr> callee) {
    std::vector<std::unique_ptr<Expr>> arguments;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            arguments.push_back(expression());
        } while (match({TokenType::COMMA}));
    }
    Token paren = consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");
    return std::make_unique<CallExpr>(std::move(callee), paren, std::move(arguments));
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
    return call();
}

std::unique_ptr<Expr> Parser::primary() {
    if (match({TokenType::FALSE})) return std::make_unique<LiteralExpr>(false);
    if (match({TokenType::TRUE})) return std::make_unique<LiteralExpr>(true);
    if (match({TokenType::NONE})) return std::make_unique<LiteralExpr>(std::monostate{});

    if (match({TokenType::NUMBER, TokenType::STRING})) {
        return std::make_unique<LiteralExpr>(previous().literal);
    }

    if (match({TokenType::IDENTIFIER})) {
        return std::make_unique<VariableExpr>(previous());
    }

    if (match({TokenType::LEFT_PAREN})) {
        auto expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return expr;
    }

    throw error(peek(), "Expect expression.");
}

std::unique_ptr<Stmt> Parser::declaration() {
    if (match({TokenType::FUNC})) {
        return function("function");
    }
    if (match({TokenType::LET, TokenType::MUT})) {
        return varDeclaration();
    }
    return statement();
}

std::unique_ptr<Stmt> Parser::function(const std::string& kind) {
    Token name = consume(TokenType::IDENTIFIER, "Expect " + kind + " name.");
    consume(TokenType::LEFT_PAREN, "Expect '(' after " + kind + " name.");
    std::vector<Token> parameters;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            parameters.push_back(consume(TokenType::IDENTIFIER, "Expect parameter name."));
        } while (match({TokenType::COMMA}));
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");

    std::unique_ptr<TypeExpr> returnType = nullptr;
    if (match({TokenType::MINUS})) {
        consume(TokenType::GREATER, "Expect '>' after '->' for return type.");
        returnType = type();
    }

    consume(TokenType::LEFT_BRACE, "Expect '{' before " + kind + " body.");
    auto body = std::make_unique<BlockStmt>(block());
    return std::make_unique<FunctionStmt>(name, std::move(parameters), std::move(returnType), std::move(body));
}

std::unique_ptr<Stmt> Parser::returnStatement() {
    Token keyword = previous();
    std::unique_ptr<Expr> value = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after return value.");
    return std::make_unique<ReturnStmt>(keyword, std::move(value));
}

std::unique_ptr<Stmt> Parser::varDeclaration() {
    Token keyword = previous();
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");
    std::unique_ptr<TypeExpr> type = nullptr;
    if (match({TokenType::COLON})) {
        type = this->type();
    }
    std::unique_ptr<Expr> initializer = nullptr;
    if (match({TokenType::EQUAL})) {
        initializer = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_unique<VarStmt>(keyword, name, std::move(type), std::move(initializer));
}

std::unique_ptr<Stmt> Parser::statement() {
    if (match({TokenType::LEFT_BRACE})) {
        return std::make_unique<BlockStmt>(block());
    }
    if (match({TokenType::RETURN})) {
        return returnStatement();
    }
    return expressionStatement();
}

std::unique_ptr<Stmt> Parser::expressionStatement() {
    auto expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    return std::make_unique<ExpressionStmt>(std::move(expr));
}

std::vector<std::unique_ptr<Stmt>> Parser::block() {
    std::vector<std::unique_ptr<Stmt>> statements;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        statements.push_back(statement());
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
    return statements;
}

std::unique_ptr<TypeExpr> Parser::type() {
    if (match({TokenType::INT, TokenType::UINT, TokenType::UINT8, TokenType::UINT16, TokenType::UINT32, TokenType::UINT64,
                 TokenType::INT8, TokenType::INT16, TokenType::INT32, TokenType::INT64, TokenType::CHAR, TokenType::DOUBLE,
                 TokenType::FLOAT, TokenType::LONG_DOUBLE, TokenType::VOID, TokenType::BOOL, TokenType::STRING_TYPE})) {
        return std::make_unique<BaseTypeExpr>(previous());
    }
    throw error(peek(), "Expect type.");
}

bool Parser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw error(peek(), message);
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
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

Parser::ParseError Parser::error(const Token& token, const std::string& message) {
    // In a real compiler, you would report the error to the user.
    // For now, we'll just throw an exception.
    return ParseError(message);
}

void Parser::synchronize() {
    advance();
    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return;
        switch (peek().type) {
            case TokenType::FUNC:
            case TokenType::LET:
            case TokenType::FOR:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::RETURN:
                return;
        }
        advance();
    }
}

} // namespace chtholly
