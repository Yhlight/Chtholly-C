#include "Parser.h"
#include "Error.h"
#include <vector>
#include "Type.h"

Parser::Parser(const std::vector<Token>& tokens) : tokens_(tokens) {}

std::vector<std::unique_ptr<Stmt>> Parser::parse() {
    std::vector<std::unique_ptr<Stmt>> statements;
    while (!isAtEnd()) {
        statements.push_back(declaration());
    }
    return statements;
}

std::unique_ptr<Stmt> Parser::declaration() {
    try {
        if (match({TokenType::FUNC})) return functionDeclaration();
        if (match({TokenType::LET, TokenType::MUT})) return letDeclaration();
        return statement();
    } catch (ParseError& error) {
        synchronize();
        return nullptr;
    }
}

std::unique_ptr<Stmt> Parser::functionDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect function name.");
    consume(TokenType::LEFT_PAREN, "Expect '(' after function name.");
    std::vector<std::pair<Token, std::unique_ptr<Type>>> parameters;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            if (parameters.size() >= 255) {
                ErrorReporter::report(peek().line, "Cannot have more than 255 parameters.");
            }
            Token paramName = consume(TokenType::IDENTIFIER, "Expect parameter name.");
            consume(TokenType::COLON, "Expect ':' after parameter name.");
            parameters.emplace_back(paramName, type());
        } while (match({TokenType::COMMA}));
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");

    std::unique_ptr<Type> returnType = nullptr;
    if (match({TokenType::ARROW})) {
        returnType = type();
    }

    consume(TokenType::LEFT_BRACE, "Expect '{' before function body.");
    std::unique_ptr<Stmt> body = std::make_unique<Block>(block());
    return std::make_unique<FuncStmt>(name, std::move(parameters), std::move(returnType), std::move(body));
}

std::unique_ptr<Stmt> Parser::letDeclaration() {
    bool isMutable = previous().type == TokenType::MUT;
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");

    std::unique_ptr<Type> typeAnnotation = nullptr;
    if (match({TokenType::COLON})) {
        typeAnnotation = type();
    }

    std::unique_ptr<Expr> initializer = nullptr;
    if (match({TokenType::EQUAL})) {
        initializer = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_unique<LetStmt>(name, std::move(typeAnnotation), std::move(initializer), isMutable);
}

std::unique_ptr<Stmt> Parser::statement() {
    if (match({TokenType::IF})) return ifStatement();
    if (match({TokenType::WHILE})) return whileStatement();
    if (match({TokenType::RETURN})) return returnStatement();
    if (match({TokenType::LEFT_BRACE})) return std::make_unique<Block>(block());
    return expressionStatement();
}

std::unique_ptr<Stmt> Parser::ifStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
    std::unique_ptr<Expr> condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");

    std::unique_ptr<Stmt> thenBranch = statement();
    std::unique_ptr<Stmt> elseBranch = nullptr;
    if (match({TokenType::ELSE})) {
        elseBranch = statement();
    }

    return std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

std::unique_ptr<Stmt> Parser::whileStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
    std::unique_ptr<Expr> condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");
    std::unique_ptr<Stmt> body = statement();

    return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
}

std::unique_ptr<Stmt> Parser::returnStatement() {
    Token keyword = previous();
    std::unique_ptr<Expr> value = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        value = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after return value.");
    return std::make_unique<ReturnStmt>(keyword, std::move(value));
}

std::unique_ptr<Stmt> Parser::expressionStatement() {
    std::unique_ptr<Expr> expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    return std::make_unique<ExpressionStmt>(std::move(expr));
}

std::vector<std::unique_ptr<Stmt>> Parser::block() {
    std::vector<std::unique_ptr<Stmt>> statements;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        statements.push_back(declaration());
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
    return statements;
}

std::unique_ptr<Type> Parser::type() {
    if (match({TokenType::IDENTIFIER, TokenType::INT, TokenType::INT8, TokenType::INT16,
                 TokenType::INT32, TokenType::INT64, TokenType::UINT8, TokenType::UINT16,
                 TokenType::UINT32, TokenType::UINT64, TokenType::CHAR_TYPE, TokenType::DOUBLE,
                 TokenType::FLOAT, TokenType::LONG, TokenType::VOID, TokenType::BOOL,
                 TokenType::STRING_TYPE, TokenType::ARRAY, TokenType::STRUCT})) {
        return std::make_unique<PrimitiveType>(previous().lexeme);
    }
    throw error(peek(), "Expect a type name.");
}

std::unique_ptr<Expr> Parser::expression() {
    return assignment();
}

std::unique_ptr<Expr> Parser::assignment() {
    std::unique_ptr<Expr> expr = equality();

    if (match({TokenType::EQUAL})) {
        Token equals = previous();
        std::unique_ptr<Expr> value = assignment();

        if (dynamic_cast<Variable*>(expr.get())) {
            Token name = static_cast<Variable*>(expr.get())->name;
            return std::make_unique<Assign>(name, std::move(value));
        }

        throw error(equals, "Invalid assignment target.");
    }

    return expr;
}

std::unique_ptr<Expr> Parser::equality() {
    std::unique_ptr<Expr> expr = comparison();
    while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
        Token op = previous();
        std::unique_ptr<Expr> right = comparison();
        expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::comparison() {
    std::unique_ptr<Expr> expr = term();
    while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL})) {
        Token op = previous();
        std::unique_ptr<Expr> right = term();
        expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::term() {
    std::unique_ptr<Expr> expr = factor();
    while (match({TokenType::MINUS, TokenType::PLUS})) {
        Token op = previous();
        std::unique_ptr<Expr> right = factor();
        expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::factor() {
    std::unique_ptr<Expr> expr = unary();
    while (match({TokenType::SLASH, TokenType::STAR})) {
        Token op = previous();
        std::unique_ptr<Expr> right = unary();
        expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::unary() {
    if (match({TokenType::BANG, TokenType::MINUS})) {
        Token op = previous();
        std::unique_ptr<Expr> right = unary();
        return std::make_unique<Unary>(op, std::move(right));
    }
    return call();
}

std::unique_ptr<Expr> Parser::call() {
    std::unique_ptr<Expr> expr = primary();

    while (true) {
        if (match({TokenType::LEFT_PAREN})) {
            std::vector<std::unique_ptr<Expr>> arguments;
            if (!check(TokenType::RIGHT_PAREN)) {
                do {
                    if (arguments.size() >= 255) {
                        throw error(peek(), "Cannot have more than 255 arguments.");
                    }
                    arguments.push_back(expression());
                } while (match({TokenType::COMMA}));
            }
            Token paren = consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");
            expr = std::make_unique<Call>(std::move(expr), paren, std::move(arguments));
        } else {
            break;
        }
    }

    return expr;
}

std::unique_ptr<Expr> Parser::primary() {
    if (match({TokenType::NUMBER, TokenType::STRING, TokenType::CHAR})) {
        return std::make_unique<Literal>(previous());
    }
    if (match({TokenType::IDENTIFIER})) {
        return std::make_unique<Variable>(previous());
    }
    if (match({TokenType::LEFT_PAREN})) {
        std::unique_ptr<Expr> expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_unique<Grouping>(std::move(expr));
    }
    throw error(peek(), "Expect expression.");
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

Parser::ParseError Parser::error(const Token& token, const std::string& message) {
    ErrorReporter::report(token.line, " at '" + token.lexeme + "' " + message);
    return ParseError();
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

Token Parser::advance() {
    if (!isAtEnd()) current_++;
    return previous();
}

bool Parser::isAtEnd() {
    return peek().type == TokenType::END_OF_FILE;
}

Token Parser::peek() {
    return tokens_[current_];
}

Token Parser::previous() {
    return tokens_[current_ - 1];
}

void Parser::synchronize() {
    advance();
    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return;
        switch (peek().type) {
            case TokenType::FUNC:
            case TokenType::LET:
            case TokenType::SWITCH:
            case TokenType::WHILE:
            case TokenType::RETURN:
                return;
        }
        advance();
    }
}
