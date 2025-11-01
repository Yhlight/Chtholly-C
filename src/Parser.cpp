#include "Parser.h"
#include <vector>

namespace chtholly {

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

std::vector<std::unique_ptr<Stmt>> Parser::parse() {
    std::vector<std::unique_ptr<Stmt>> statements;
    while (!isAtEnd()) {
        std::unique_ptr<Stmt> stmt = declaration();
        if (stmt) {
            statements.push_back(std::move(stmt));
        }
    }
    return statements;
}

std::unique_ptr<Stmt> Parser::declaration() {
    try {
        if (match({TokenType::STRUCT})) return structDeclaration();
        if (match({TokenType::FUNC})) return function();
        if (match({TokenType::LET, TokenType::MUT})) {
            std::unique_ptr<Stmt> stmt = varDeclaration();
            consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
            return stmt;
        }
        return statement();
    } catch (ParseError& error) {
        synchronize();
        return nullptr;
    }
}

std::unique_ptr<Stmt> Parser::statement() {
    if (match({TokenType::FOR})) return forStatement();
    if (match({TokenType::IF})) return ifStatement();
    if (match({TokenType::RETURN})) return returnStatement();
    if (match({TokenType::SWITCH})) return switchStatement();
    if (match({TokenType::BREAK})) return breakStatement();
    if (match({TokenType::FALLTHROUGH})) return fallthroughStatement();
    if (match({TokenType::LEFT_BRACE})) return std::make_unique<BlockStmt>(block());

    std::unique_ptr<Stmt> stmt = expressionStatement();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    return stmt;
}

std::unique_ptr<Stmt> Parser::varDeclaration() {
    Token keyword = previous(); // The 'let' or 'mut' token
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");

    std::optional<Token> type;
    if (match({TokenType::COLON})) {
        type = consume(TokenType::IDENTIFIER, "Expect type annotation.");
    }

    std::unique_ptr<Expr> initializer = nullptr;
    if (match({TokenType::EQUAL})) {
        initializer = expression();
    }
    return std::make_unique<VarDeclStmt>(std::move(keyword), std::move(name), std::move(type), std::move(initializer));
}

std::unique_ptr<Stmt> Parser::expressionStatement() {
    std::unique_ptr<Expr> expr = expression();
    return std::make_unique<ExprStmt>(std::move(expr));
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

std::unique_ptr<Stmt> Parser::forStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'.");

    std::unique_ptr<Stmt> initializer;
    if (match({TokenType::SEMICOLON})) {
        initializer = nullptr;
    } else if (match({TokenType::LET, TokenType::MUT})) {
        initializer = varDeclaration();
        consume(TokenType::SEMICOLON, "Expect ';' after loop initializer.");
    } else {
        initializer = expressionStatement();
        consume(TokenType::SEMICOLON, "Expect ';' after loop initializer.");
    }

    std::unique_ptr<Expr> condition = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        condition = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");

    std::unique_ptr<Expr> increment = nullptr;
    if (!check(TokenType::RIGHT_PAREN)) {
        increment = expression();
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");

    std::unique_ptr<Stmt> body = statement();

    return std::make_unique<ForStmt>(std::move(initializer), std::move(condition), std::move(increment), std::move(body));
}

std::unique_ptr<Stmt> Parser::function() {
    Token name = consume(TokenType::IDENTIFIER, "Expect function name.");
    consume(TokenType::LEFT_PAREN, "Expect '(' after function name.");

    std::vector<FuncDeclStmt::Param> params;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            Token paramName = consume(TokenType::IDENTIFIER, "Expect parameter name.");
            consume(TokenType::COLON, "Expect ':' after parameter name.");
            Token paramType = consume(TokenType::IDENTIFIER, "Expect parameter type.");
            params.push_back({std::move(paramName), std::move(paramType)});
        } while (match({TokenType::COMMA}));
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");

    std::optional<Token> returnType;
    if (match({TokenType::ARROW})) {
        returnType = consume(TokenType::IDENTIFIER, "Expect return type.");
    }

    consume(TokenType::LEFT_BRACE, "Expect '{' before function body.");
    auto body = std::make_unique<BlockStmt>(block());

    return std::make_unique<FuncDeclStmt>(std::move(name), std::move(params), std::move(returnType), std::move(body));
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

std::unique_ptr<Stmt> Parser::structDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect struct name.");
    consume(TokenType::LEFT_BRACE, "Expect '{' before struct body.");

    std::vector<StructDeclStmt::MemberVar> members;
    std::vector<std::unique_ptr<FuncDeclStmt>> methods;

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        StructDeclStmt::Visibility visibility = StructDeclStmt::Visibility::PUBLIC;
        if (match({TokenType::PUBLIC})) {
            // Default is public, so this is optional
        } else if (match({TokenType::PRIVATE})) {
            visibility = StructDeclStmt::Visibility::PRIVATE;
        }

        if (check(TokenType::FUNC)) {
            advance();
            methods.push_back(std::unique_ptr<FuncDeclStmt>(static_cast<FuncDeclStmt*>(function().release())));
        } else {
            Token memberName = consume(TokenType::IDENTIFIER, "Expect member name.");
            consume(TokenType::COLON, "Expect ':' after member name.");
            Token memberType = consume(TokenType::IDENTIFIER, "Expect member type.");
            consume(TokenType::SEMICOLON, "Expect ';' after member declaration.");
            members.push_back({visibility, std::move(memberName), std::move(memberType)});
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after struct body.");
    return std::make_unique<StructDeclStmt>(std::move(name), std::move(members), std::move(methods));
}

std::unique_ptr<Stmt> Parser::switchStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'switch'.");
    std::unique_ptr<Expr> condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after switch condition.");
    consume(TokenType::LEFT_BRACE, "Expect '{' before switch cases.");

    std::vector<std::unique_ptr<CaseStmt>> cases;
    while (!match({TokenType::RIGHT_BRACE}) && !isAtEnd()) {
        if (match({TokenType::CASE})) {
            std::unique_ptr<Expr> caseCondition = expression();
            consume(TokenType::COLON, "Expect ':' after case value.");
            std::unique_ptr<Stmt> body = statement();
            cases.push_back(std::make_unique<CaseStmt>(std::move(caseCondition), std::move(body)));
        } else if (match({TokenType::DEFAULT})) {
            consume(TokenType::COLON, "Expect ':' after default.");
            std::unique_ptr<Stmt> body = statement();
            cases.push_back(std::make_unique<CaseStmt>(nullptr, std::move(body)));
        } else {
            throw error(peek(), "Expect 'case' or 'default'.");
        }
    }

    return std::make_unique<SwitchStmt>(std::move(condition), std::move(cases));
}

std::unique_ptr<Stmt> Parser::breakStatement() {
    Token keyword = previous();
    consume(TokenType::SEMICOLON, "Expect ';' after 'break'.");
    return std::make_unique<BreakStmt>(std::move(keyword));
}

std::unique_ptr<Stmt> Parser::fallthroughStatement() {
    Token keyword = previous();
    consume(TokenType::SEMICOLON, "Expect ';' after 'fallthrough'.");
    return std::make_unique<FallthroughStmt>(std::move(keyword));
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
    std::unique_ptr<Expr> expr = logical_or();

    if (match({TokenType::EQUAL})) {
        Token equals = previous();
        std::unique_ptr<Expr> value = assignment();

        if (auto* varExpr = dynamic_cast<VariableExpr*>(expr.get())) {
            return std::make_unique<BinaryExpr>(std::move(expr), std::move(equals), std::move(value));
        }

        throw error(equals, "Invalid assignment target.");
    }

    return expr;
}

std::unique_ptr<Expr> Parser::logical_or() {
    std::unique_ptr<Expr> expr = logical_and();

    while (match({TokenType::OR})) {
        Token op = previous();
        std::unique_ptr<Expr> right = logical_and();
        expr = std::make_unique<BinaryExpr>(std::move(expr), std::move(op), std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::logical_and() {
    std::unique_ptr<Expr> expr = comparison();

    while (match({TokenType::AND})) {
        Token op = previous();
        std::unique_ptr<Expr> right = comparison();
        expr = std::make_unique<BinaryExpr>(std::move(expr), std::move(op), std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::comparison() {
    std::unique_ptr<Expr> expr = term();

    while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL, TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
        Token op = previous();
        std::unique_ptr<Expr> right = term();
        expr = std::make_unique<BinaryExpr>(std::move(expr), std::move(op), std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::term() {
    std::unique_ptr<Expr> expr = factor();

    while (match({TokenType::MINUS, TokenType::PLUS})) {
        Token op = previous();
        std::unique_ptr<Expr> right = factor();
        expr = std::make_unique<BinaryExpr>(std::move(expr), std::move(op), std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::factor() {
    std::unique_ptr<Expr> expr = unary();

    while (match({TokenType::SLASH, TokenType::STAR})) {
        Token op = previous();
        std::unique_ptr<Expr> right = unary();
        expr = std::make_unique<BinaryExpr>(std::move(expr), std::move(op), std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::unary() {
    if (match({TokenType::BANG, TokenType::MINUS})) {
        Token op = previous();
        std::unique_ptr<Expr> right = unary();
        return std::make_unique<UnaryExpr>(std::move(op), std::move(right));
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
                    arguments.push_back(expression());
                } while (match({TokenType::COMMA}));
            }
            Token paren = consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");
            expr = std::make_unique<CallExpr>(std::move(expr), std::move(paren), std::move(arguments));
        } else if (match({TokenType::DOT})) {
            Token name = consume(TokenType::IDENTIFIER, "Expect property name after '.'.");
            expr = std::make_unique<MemberAccessExpr>(std::move(expr), std::move(name));
        } else {
            break;
        }
    }

    return expr;
}


std::unique_ptr<Expr> Parser::primary() {
    if (match({TokenType::FALSE, TokenType::TRUE})) {
        return std::make_unique<LiteralExpr>(previous());
    }
    if (match({TokenType::NUMBER, TokenType::STRING})) {
        return std::make_unique<LiteralExpr>(previous());
    }
    if (match({TokenType::IDENTIFIER})) {
        if (check(TokenType::LEFT_BRACE)) {
            Token name = previous();
            consume(TokenType::LEFT_BRACE, "Expect '{' after struct name.");
            std::vector<StructInitExpr::MemberInit> members;
            if (!check(TokenType::RIGHT_BRACE)) {
                do {
                    Token memberName = consume(TokenType::IDENTIFIER, "Expect member name.");
                    consume(TokenType::COLON, "Expect ':' after member name.");
                    std::unique_ptr<Expr> initializer = expression();
                    members.push_back({std::move(memberName), std::move(initializer)});
                } while (match({TokenType::COMMA}));
            }
            consume(TokenType::RIGHT_BRACE, "Expect '}' after struct members.");
            return std::make_unique<StructInitExpr>(std::move(name), std::move(members));
        }
        return std::make_unique<VariableExpr>(previous());
    }
    if (match({TokenType::LEFT_BRACKET})) {
        Token bracket = previous();
        // Captures are not supported yet.
        consume(TokenType::RIGHT_BRACKET, "Expect ']' after capture list.");
        consume(TokenType::LEFT_PAREN, "Expect '(' after '[]'.");
        std::vector<FuncDeclStmt::Param> params;
        if (!check(TokenType::RIGHT_PAREN)) {
            do {
                Token paramName = consume(TokenType::IDENTIFIER, "Expect parameter name.");
                consume(TokenType::COLON, "Expect ':' after parameter name.");
                Token paramType = consume(TokenType::IDENTIFIER, "Expect parameter type.");
                params.push_back({std::move(paramName), std::move(paramType)});
            } while (match({TokenType::COMMA}));
        }
        consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");

        std::optional<Token> returnType;
        if (match({TokenType::ARROW})) {
            returnType = consume(TokenType::IDENTIFIER, "Expect return type.");
        }

        consume(TokenType::LEFT_BRACE, "Expect '{' before lambda body.");
        auto body = std::make_unique<BlockStmt>(block());
        return std::make_unique<LambdaExpr>(std::move(bracket), std::move(params), std::move(returnType), std::move(body));
    }
    if (match({TokenType::LEFT_PAREN})) {
        std::unique_ptr<Expr> expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_unique<GroupingExpr>(std::move(expr));
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

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

const Token& Parser::peek() const {
    return tokens[current];
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::END_OF_FILE;
}

const Token& Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

const Token& Parser::previous() const {
    return tokens[current - 1];
}

const Token& Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw error(peek(), message);
}

ParseError Parser::error(const Token& token, const std::string& message) const {
    return ParseError("Error at token " + token.lexeme + ": " + message);
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
            case TokenType::SWITCH:
            case TokenType::RETURN:
                return;
        }

        advance();
    }
}

} // namespace chtholly
