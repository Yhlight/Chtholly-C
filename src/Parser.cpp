#include "Parser.h"
#include <stdexcept>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

std::vector<std::shared_ptr<Stmt>> Parser::parse() {
    std::vector<std::shared_ptr<Stmt>> statements;
    while (!isAtEnd()) {
        statements.push_back(declaration());
    }
    return statements;
}

std::shared_ptr<Stmt> Parser::declaration() {
    try {
        if (match({TokenType::STRUCT})) return structDeclaration();
        if (match({TokenType::FUNC})) return function("function");
        if (match({TokenType::LET, TokenType::MUT})) {
            return varDeclaration();
        }
        return statement();
    } catch (const std::runtime_error& error) {
        synchronize();
        return nullptr;
    }
}

std::shared_ptr<Stmt> Parser::varDeclaration() {
    bool is_mutable = previous().type == TokenType::MUT;
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");
    std::shared_ptr<Type> varType = nullptr;
    if (check(TokenType::COLON)) {
        varType = type();
    }
    std::shared_ptr<Expr> initializer = nullptr;
    if (match({TokenType::EQUAL})) {
        initializer = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_shared<Var>(name, varType, initializer, is_mutable);
}

std::shared_ptr<Stmt> Parser::structDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect struct name.");
    consume(TokenType::LEFT_BRACE, "Expect '{' before struct body.");

    std::vector<std::shared_ptr<Var>> fields;
    std::vector<std::shared_ptr<Func>> methods;

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        // TODO: Access modifiers public, private
        if (current + 1 < tokens.size() && peek().type == TokenType::IDENTIFIER && tokens[current + 1].type == TokenType::LEFT_PAREN) {
            methods.push_back(std::dynamic_pointer_cast<Func>(function("method")));
        } else if (match({TokenType::LET, TokenType::MUT})) {
            fields.push_back(std::dynamic_pointer_cast<Var>(varDeclaration()));
        } else {
            throw std::runtime_error("Unexpected token '" + peek().lexeme + "' in struct body.");
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after struct body.");
    return std::make_shared<Struct>(name, fields, methods);
}

std::shared_ptr<Stmt> Parser::statement() {
    if (match({TokenType::IF})) {
        return ifStatement();
    }
    if (match({TokenType::SWITCH})) {
        return switchStatement();
    }
    if (match({TokenType::WHILE})) {
        return whileStatement();
    }
    if (match({TokenType::FOR})) {
        return forStatement();
    }
    if (match({TokenType::RETURN})) {
        return returnStatement();
    }
    if (match({TokenType::BREAK})) {
        return breakStatement();
    }
    if (match({TokenType::FALLTHROUGH})) {
        return fallthroughStatement();
    }
    if (match({TokenType::LEFT_BRACE})) {
        return std::make_shared<Block>(block());
    }
    return expressionStatement();
}

std::shared_ptr<Stmt> Parser::ifStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
    std::shared_ptr<Expr> condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");

    std::shared_ptr<Stmt> thenBranch = statement();
    std::shared_ptr<Stmt> elseBranch = nullptr;
    if (match({TokenType::ELSE})) {
        elseBranch = statement();
    }

    return std::make_shared<If>(condition, thenBranch, elseBranch);
}

std::shared_ptr<Stmt> Parser::whileStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
    std::shared_ptr<Expr> condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after while condition.");
    std::shared_ptr<Stmt> body = statement();

    return std::make_shared<While>(condition, body);
}

std::shared_ptr<Stmt> Parser::forStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'.");

    std::shared_ptr<Stmt> initializer;
    if (match({TokenType::SEMICOLON})) {
        initializer = nullptr;
    } else if (match({TokenType::LET, TokenType::MUT})) {
        initializer = varDeclaration();
    } else {
        initializer = expressionStatement();
    }

    std::shared_ptr<Expr> condition = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        condition = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");

    std::shared_ptr<Expr> increment = nullptr;
    if (!check(TokenType::RIGHT_PAREN)) {
        increment = expression();
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");

    std::shared_ptr<Stmt> body = statement();

    return std::make_shared<For>(initializer, condition, increment, body);
}

std::shared_ptr<Stmt> Parser::function(std::string kind) {
    Token name = consume(TokenType::IDENTIFIER, "Expect " + kind + " name.");
    consume(TokenType::LEFT_PAREN, "Expect '(' after " + kind + " name.");
    std::vector<Parameter> parameters;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            if (parameters.size() >= 255) {
                throw std::runtime_error("Can't have more than 255 parameters.");
            }
            Token paramName = consume(TokenType::IDENTIFIER, "Expect parameter name.");
            std::shared_ptr<Type> paramType = nullptr;
            if (check(TokenType::COLON)) {
                consume(TokenType::COLON, "Expect ':' before type.");
                bool is_ref = false;
                bool is_mut_ref = false;
                if (match({TokenType::AMPERSAND})) {
                    is_ref = true;
                    if (match({TokenType::MUT})) {
                        is_mut_ref = true;
                    }
                }
                Token typeName = consume(TokenType::IDENTIFIER, "Expect type name.");
                paramType = std::make_shared<Type>(Type{typeName, is_ref, is_mut_ref});
            }
            parameters.push_back({paramName, paramType});
        } while (match({TokenType::COMMA}));
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");

    std::shared_ptr<Type> returnType = nullptr;
    if (match({TokenType::MINUS})) {
        consume(TokenType::GREATER, "Expect '>' after '-' for return type arrow.");
        returnType = type();
    }

    consume(TokenType::LEFT_BRACE, "Expect '{' before " + kind + " body.");
    std::vector<std::shared_ptr<Stmt>> body = block();
    return std::make_shared<Func>(name, parameters, body, returnType);
}

std::shared_ptr<Stmt> Parser::returnStatement() {
    Token keyword = previous();
    std::shared_ptr<Expr> value = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        value = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after return value.");
    return std::make_shared<Return>(keyword, value);
}

std::vector<std::shared_ptr<Stmt>> Parser::block() {
    std::vector<std::shared_ptr<Stmt>> statements;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        statements.push_back(declaration());
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
    return statements;
}

std::shared_ptr<Stmt> Parser::expressionStatement() {
    std::shared_ptr<Expr> expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    return std::make_shared<Expression>(expr);
}

std::shared_ptr<Stmt> Parser::switchStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'switch'.");
    std::shared_ptr<Expr> condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after switch condition.");
    consume(TokenType::LEFT_BRACE, "Expect '{' before switch cases.");

    std::vector<std::shared_ptr<Case>> cases;
    while (match({TokenType::CASE})) {
        std::shared_ptr<Expr> case_condition = expression();
        consume(TokenType::COLON, "Expect ':' after case condition.");
        std::shared_ptr<Stmt> body = statement();
        cases.push_back(std::make_shared<Case>(case_condition, body));
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after switch cases.");
    return std::make_shared<Switch>(condition, cases);
}

std::shared_ptr<Stmt> Parser::breakStatement() {
    Token keyword = previous();
    consume(TokenType::SEMICOLON, "Expect ';' after 'break'.");
    return std::make_shared<Break>(keyword);
}

std::shared_ptr<Stmt> Parser::fallthroughStatement() {
    Token keyword = previous();
    consume(TokenType::SEMICOLON, "Expect ';' after 'fallthrough'.");
    return std::make_shared<Fallthrough>(keyword);
}

std::shared_ptr<Expr> Parser::expression() {
    return assignment();
}

std::shared_ptr<Expr> Parser::assignment() {
    std::shared_ptr<Expr> expr = logical_or();
    if (match({TokenType::EQUAL})) {
        Token equals = previous();
        std::shared_ptr<Expr> value = assignment();
        return std::make_shared<Assign>(expr, value);
    }
    return expr;
}

std::shared_ptr<Expr> Parser::logical_or() {
    std::shared_ptr<Expr> expr = logical_and();
    while (match({TokenType::PIPE_PIPE})) {
        Token op = previous();
        std::shared_ptr<Expr> right = logical_and();
        expr = std::make_shared<Binary>(expr, op, right);
    }
    return expr;
}

std::shared_ptr<Expr> Parser::logical_and() {
    std::shared_ptr<Expr> expr = equality();
    while (match({TokenType::AMPERSAND_AMPERSAND})) {
        Token op = previous();
        std::shared_ptr<Expr> right = equality();
        expr = std::make_shared<Binary>(expr, op, right);
    }
    return expr;
}

std::shared_ptr<Expr> Parser::equality() {
    std::shared_ptr<Expr> expr = comparison();
    while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
        Token op = previous();
        std::shared_ptr<Expr> right = comparison();
        expr = std::make_shared<Binary>(expr, op, right);
    }
    return expr;
}

std::shared_ptr<Expr> Parser::comparison() {
    std::shared_ptr<Expr> expr = term();
    while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL})) {
        Token op = previous();
        std::shared_ptr<Expr> right = term();
        expr = std::make_shared<Binary>(expr, op, right);
    }
    return expr;
}

std::shared_ptr<Expr> Parser::term() {
    std::shared_ptr<Expr> expr = factor();
    while (match({TokenType::MINUS, TokenType::PLUS})) {
        Token op = previous();
        std::shared_ptr<Expr> right = factor();
        expr = std::make_shared<Binary>(expr, op, right);
    }
    return expr;
}

std::shared_ptr<Expr> Parser::factor() {
    std::shared_ptr<Expr> expr = unary();
    while (match({TokenType::SLASH, TokenType::STAR})) {
        Token op = previous();
        std::shared_ptr<Expr> right = unary();
        expr = std::make_shared<Binary>(expr, op, right);
    }
    return expr;
}

std::shared_ptr<Expr> Parser::unary() {
    if (match({TokenType::BANG, TokenType::MINUS})) {
        Token op = previous();
        std::shared_ptr<Expr> right = unary();
        return std::make_shared<Unary>(op, right);
    }
    if (match({TokenType::AMPERSAND})) {
        Token op = previous();
        std::shared_ptr<Expr> right = unary();
        return std::make_shared<Reference>(op, right);
    }
    if (match({TokenType::STAR})) {
        Token op = previous();
        std::shared_ptr<Expr> right = unary();
        return std::make_shared<Dereference>(op, right);
    }
    return call();
}

std::shared_ptr<Expr> Parser::call() {
    std::shared_ptr<Expr> expr = primary();

    while (true) {
        if (match({TokenType::LEFT_PAREN})) {
            expr = finishCall(expr);
        } else if (match({TokenType::DOT})) {
            Token name = consume(TokenType::IDENTIFIER, "Expect property name after '.'.");
            expr = std::make_shared<Get>(expr, name);
        } else {
            break;
        }
    }

    return expr;
}

std::shared_ptr<Expr> Parser::finishCall(std::shared_ptr<Expr> callee) {
    std::vector<std::shared_ptr<Expr>> arguments;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            if (arguments.size() >= 255) {
                throw std::runtime_error("Can't have more than 255 arguments.");
            }
            arguments.push_back(assignment());
        } while (match({TokenType::COMMA}));
    }

    Token paren = consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");

    return std::make_shared<Call>(callee, paren, arguments);
}

std::shared_ptr<Expr> Parser::primary() {
    if (match({TokenType::NUMBER, TokenType::STRING})) {
        return std::make_shared<Literal>(previous().lexeme, previous().type);
    }
     if (match({TokenType::SELF})) {
        return std::make_shared<Self>(previous());
    }
    if (match({TokenType::IDENTIFIER})) {
        if (check(TokenType::LEFT_BRACE)) {
            return structInstantiation();
        }
        return std::make_shared<Variable>(previous());
    }
    if (match({TokenType::LEFT_PAREN})) {
        std::shared_ptr<Expr> expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_shared<Grouping>(expr);
    }
    throw std::runtime_error("Expect expression.");
}

std::shared_ptr<Expr> Parser::structInstantiation() {
    Token name = previous();
    consume(TokenType::LEFT_BRACE, "Expect '{' after struct name.");
    std::vector<std::pair<Token, std::shared_ptr<Expr>>> fields;
    if (!check(TokenType::RIGHT_BRACE)) {
        do {
            Token fieldName = consume(TokenType::IDENTIFIER, "Expect field name.");
            consume(TokenType::COLON, "Expect ':' after field name.");
            std::shared_ptr<Expr> value = expression();
            fields.push_back({fieldName, value});
        } while (match({TokenType::COMMA}));
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after struct fields.");
    return std::make_shared<Instantiation>(name, fields);
}

std::shared_ptr<Type> Parser::type() {
    consume(TokenType::COLON, "Expect ':' before type.");
    bool is_ref = false;
    bool is_mut_ref = false;
    if (match({TokenType::AMPERSAND})) {
        is_ref = true;
        if (match({TokenType::MUT})) {
            is_mut_ref = true;
        }
    }
    Token typeName = consume(TokenType::IDENTIFIER, "Expect type name.");
    return std::make_shared<Type>(Type{typeName, is_ref, is_mut_ref});
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
    throw std::runtime_error(message);
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
