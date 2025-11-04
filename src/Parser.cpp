#include "Parser.h"
#include <utility>
#include <iostream>

namespace chtholly {

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

std::vector<std::unique_ptr<Stmt>> Parser::parse() {
    std::vector<std::unique_ptr<Stmt>> statements;
    while (!isAtEnd()) {
        statements.push_back(declaration());
    }
    return statements;
}

std::unique_ptr<Stmt> Parser::declaration() {
    try {
        if (match(TokenType::FUNC)) return functionDeclaration("function");
        if (match(TokenType::STRUCT)) return structDeclaration();
        if (match(TokenType::LET, TokenType::MUT)) return varDeclaration();
        return statement();
    } catch (ParseError& error) {
        synchronize();
        return nullptr;
    }
}

std::unique_ptr<Stmt> Parser::varDeclaration() {
    Token keyword = previous();
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");
    std::unique_ptr<TypeExpr> type = nullptr;
    // if (match(TokenType::COLON)) {
    //     type = typeAnnotation();
    // }
    std::unique_ptr<Expr> initializer = nullptr;
    if (match(TokenType::EQUAL)) {
        initializer = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_unique<VarStmt>(std::move(name), std::move(type), std::move(initializer), keyword.type == TokenType::MUT);
}

std::unique_ptr<Stmt> Parser::statement() {
    if (match(TokenType::IF)) return ifStatement();
    if (match(TokenType::WHILE)) return whileStatement();
    if (match(TokenType::SWITCH)) return switchStatement();
    if (match(TokenType::RETURN)) return returnStatement();
    if (match(TokenType::IMPORT)) return importStatement();
    if (match(TokenType::LEFT_BRACE)) return std::make_unique<BlockStmt>(block());
    return expressionStatement();
}

std::unique_ptr<Stmt> Parser::expressionStatement() {
    auto expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    return std::make_unique<ExpressionStmt>(std::move(expr));
}

std::unique_ptr<Stmt> Parser::ifStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
    auto condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");
    auto thenBranch = statement();
    std::unique_ptr<Stmt> elseBranch = nullptr;
    if (match(TokenType::ELSE)) {
        elseBranch = statement();
    }
    return std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

std::unique_ptr<Stmt> Parser::whileStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
    auto condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after while condition.");
    auto body = statement();
    return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
}

std::unique_ptr<Stmt> Parser::switchStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'switch'.");
    auto value = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after switch value.");
    consume(TokenType::LEFT_BRACE, "Expect '{' before switch cases.");
    std::vector<std::unique_ptr<CaseStmt>> cases;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        cases.push_back(caseStatement());
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after switch cases.");
    return std::make_unique<SwitchStmt>(std::move(value), std::move(cases));
}

std::unique_ptr<CaseStmt> Parser::caseStatement() {
    consume(TokenType::CASE, "Expect 'case'.");
    auto value = expression();
    consume(TokenType::COLON, "Expect ':' after case value.");
    auto body = statement();
    return std::make_unique<CaseStmt>(std::move(value), std::move(body));
}

std::unique_ptr<Stmt> Parser::returnStatement() {
    Token keyword = previous();
    std::unique_ptr<Expr> value = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        value = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after return value.");
    return std::make_unique<ReturnStmt>(std::move(keyword), std::move(value));
}

std::unique_ptr<Stmt> Parser::importStatement() {
    Token keyword = previous();
    std::variant<std::string, Token> path;
    if (match(TokenType::STRING_LITERAL)) {
        path = std::get<std::string>(previous().literal);
    } else {
        path = consume(TokenType::IDENTIFIER, "Expect module name or file path after 'import'.");
    }
    consume(TokenType::SEMICOLON, "Expect ';' after import statement.");
    return std::make_unique<ImportStmt>(std::move(keyword), std::move(path));
}

std::unique_ptr<FunctionStmt> Parser::functionDeclaration(const std::string& kind) {
    Token name = consume(TokenType::IDENTIFIER, "Expect " + kind + " name.");
    consume(TokenType::LEFT_PAREN, "Expect '(' after " + kind + " name.");
    std::vector<Token> parameters;
    std::vector<std::unique_ptr<TypeExpr>> param_types;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            parameters.push_back(consume(TokenType::IDENTIFIER, "Expect parameter name."));
            // consume(TokenType::COLON, "Expect ':' after parameter name.");
            // param_types.push_back(typeAnnotation());
        } while (match(TokenType::COMMA));
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");
    std::unique_ptr<TypeExpr> returnType = nullptr;
    // if (match(TokenType::ARROW)) {
    //     returnType = typeAnnotation();
    // }
    consume(TokenType::LEFT_BRACE, "Expect '{' before " + kind + " body.");
    auto body = std::make_unique<BlockStmt>(block());
    return std::make_unique<FunctionStmt>(std::move(name), std::move(parameters), std::move(param_types), std::move(returnType), std::move(body));
}

std::unique_ptr<Stmt> Parser::structDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect struct name.");
    consume(TokenType::LEFT_BRACE, "Expect '{' before struct body.");
    std::vector<std::unique_ptr<VarStmt>> fields;
    std::vector<std::unique_ptr<FunctionStmt>> methods;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (match(TokenType::FUNC)) {
            methods.push_back(functionDeclaration("method"));
        } else {
            // This is a simplification. We'd need to parse field declarations.
        }
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after struct body.");
    return std::make_unique<StructStmt>(std::move(name), std::move(fields), std::move(methods));
}

std::vector<std::unique_ptr<Stmt>> Parser::block() {
    std::vector<std::unique_ptr<Stmt>> statements;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        statements.push_back(declaration());
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
    return statements;
}


std::unique_ptr<Expr> Parser::expression() {
    return assignment();
}

std::unique_ptr<Expr> Parser::assignment() {
    auto expr = equality();

    if (match(TokenType::EQUAL)) {
        Token equals = previous();
        auto value = assignment();

        if (auto varExpr = dynamic_cast<VariableExpr*>(expr.get())) {
            return std::make_unique<AssignExpr>(varExpr->name, std::move(value));
        }

        error(equals, "Invalid assignment target.");
    }

    return expr;
}

std::unique_ptr<Expr> Parser::equality() {
    auto expr = comparison();
    while (match(TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL)) {
        Token op = previous();
        auto right = comparison();
        expr = std::make_unique<BinaryExpr>(std::move(expr), std::move(op), std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::comparison() {
    auto expr = term();
    while (match(TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL)) {
        Token op = previous();
        auto right = term();
        expr = std::make_unique<BinaryExpr>(std::move(expr), std::move(op), std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::term() {
    auto expr = factor();
    while (match(TokenType::MINUS, TokenType::PLUS)) {
        Token op = previous();
        auto right = factor();
        expr = std::make_unique<BinaryExpr>(std::move(expr), std::move(op), std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::factor() {
    auto expr = unary();
    while (match(TokenType::SLASH, TokenType::STAR)) {
        Token op = previous();
        auto right = unary();
        expr = std::make_unique<BinaryExpr>(std::move(expr), std::move(op), std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::unary() {
    if (match(TokenType::BANG, TokenType::MINUS)) {
        Token op = previous();
        auto right = unary();
        return std::make_unique<UnaryExpr>(std::move(op), std::move(right));
    }
    return primary();
}

std::unique_ptr<Expr> Parser::primary() {
    if (match(TokenType::FALSE)) return std::make_unique<LiteralExpr>(false);
    if (match(TokenType::TRUE)) return std::make_unique<LiteralExpr>(true);
    if (match(TokenType::NONE)) return std::make_unique<LiteralExpr>(nullptr);

    if (match(TokenType::NUMBER_LITERAL, TokenType::STRING_LITERAL, TokenType::CHAR_LITERAL)) {
        return std::make_unique<LiteralExpr>(previous().literal);
    }

    if (match(TokenType::IDENTIFIER)) {
        return std::make_unique<VariableExpr>(previous());
    }

    if (match(TokenType::LEFT_PAREN)) {
        auto expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_unique<GroupingExpr>(std::move(expr));
    }

    throw error(peek(), "Expect expression.");
}

// Helper method implementations
bool Parser::isAtEnd() {
    return peek().type == TokenType::END_OF_FILE;
}

Token Parser::peek() {
    return tokens[current];
}

Token Parser::previous() {
    return tokens[current - 1];
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

template<typename... Args>
bool Parser::match(Args... types) {
    for (TokenType type : {types...}) {
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
    std::cerr << "ParseError at token " << token.lexeme << ": " << message << std::endl;
    return ParseError(message);
}

void Parser::synchronize() {
    advance();

    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return;

        switch (peek().type) {
            case TokenType::FUNC:
            case TokenType::LET:
            case TokenType::MUT:
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
