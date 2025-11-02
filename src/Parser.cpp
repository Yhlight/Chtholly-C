#include "../include/Parser.h"
#include <iostream>

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

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
        if (match({TokenType::FUNC})) return function("function", true);
        if (match({TokenType::LET})) return varDeclaration(false, true);
        if (match({TokenType::MUT})) return varDeclaration(true, true);
        return statement();
    } catch (ParseError& error) {
        synchronize();
        return nullptr;
    }
}

std::shared_ptr<Stmt> Parser::varDeclaration(bool isMutable, bool is_public, bool in_struct) {
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");
    std::shared_ptr<Type> type = nullptr;
    if (match({TokenType::COLON})) {
        type = parseType();
    }

    std::shared_ptr<Expr> initializer = nullptr;
    if (match({TokenType::EQUAL})) {
        initializer = expression();
    }
    else if (in_struct) {
        throw error(peek(), "Expect initializer for struct field.");
    }

    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    auto var = std::make_shared<Var>(name, initializer, isMutable, is_public);
    if (type) {
        var->type = type;
    }
    else if (initializer) {
        var->type = initializer->type;
    }

    if (var->type) {
        var_types[name.lexeme] = var->type;
    }

    return var;
}

std::shared_ptr<Stmt> Parser::statement() {
    if (match({TokenType::RETURN})) return returnStatement();
    if (match({TokenType::LEFT_BRACE})) {
        return std::make_shared<Block>(block());
    }
    return expressionStatement();
}

std::shared_ptr<Stmt> Parser::expressionStatement() {
    std::shared_ptr<Expr> expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    return std::make_shared<Expression>(expr);
}

std::vector<std::shared_ptr<Stmt>> Parser::block() {
    std::vector<std::shared_ptr<Stmt>> statements;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        statements.push_back(declaration());
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
    return statements;
}

std::shared_ptr<Expr> Parser::expression() {
    return assignment();
}

std::shared_ptr<Expr> Parser::assignment() {
    std::shared_ptr<Expr> expr = equality();
    if (match({TokenType::EQUAL})) {
        Token equals = previous();
        std::shared_ptr<Expr> value = assignment();
        if (auto var = std::dynamic_pointer_cast<Variable>(expr)) {
            Token name = var->name;
            return std::make_shared<Assign>(name, value);
        }
        error(equals, "Invalid assignment target.");
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
    while (match({TokenType::SLASH, TokenType::STAR, TokenType::PERCENT})) {
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
    return call();
}

std::shared_ptr<Expr> Parser::primary() {
    if (match({TokenType::FALSE})) {
        auto literal = std::make_shared<Literal>(false);
        literal->type = std::make_shared<PrimitiveType>("bool");
        return literal;
    }
    if (match({TokenType::TRUE})) {
        auto literal = std::make_shared<Literal>(true);
        literal->type = std::make_shared<PrimitiveType>("bool");
        return literal;
    }
    if (match({TokenType::NONE})) {
        auto literal = std::make_shared<Literal>(nullptr);
        literal->type = std::make_shared<PrimitiveType>("nullptr_t");
        return literal;
    }

    if (match({TokenType::STRING})) {
        auto literal = std::make_shared<Literal>(previous().lexeme);
        literal->type = std::make_shared<PrimitiveType>("std::string");
        return literal;
    }
    if (match({TokenType::INTEGER})) {
        auto literal = std::make_shared<Literal>(std::stoi(previous().lexeme));
        literal->type = std::make_shared<PrimitiveType>("int");
        return literal;
    }
    if (match({TokenType::DOUBLE})) {
        auto literal = std::make_shared<Literal>(std::stod(previous().lexeme));
        literal->type = std::make_shared<PrimitiveType>("double");
        return literal;
    }

    if (match({TokenType::IDENTIFIER})) {
        if (check(TokenType::LEFT_BRACE)) {
            Token name = previous();
            consume(TokenType::LEFT_BRACE, "Expect '{' after struct name for instantiation.");
            std::vector<Token> fields;
            std::vector<std::shared_ptr<Expr>> values;
            if (!check(TokenType::RIGHT_BRACE)) {
                do {
                    fields.push_back(consume(TokenType::IDENTIFIER, "Expect field name."));
                    consume(TokenType::COLON, "Expect ':' after field name.");
                    values.push_back(expression());
                } while (match({TokenType::COMMA}));
            }
            consume(TokenType::RIGHT_BRACE, "Expect '}' after struct fields.");
            return std::make_shared<Instantiation>(name, fields, values);
        }
        auto var = std::make_shared<Variable>(previous());
        if (var_types.count(var->name.lexeme)) {
            var->type = var_types[var->name.lexeme];
        }
        return var;
    }

    if (match({TokenType::LEFT_PAREN})) {
        std::shared_ptr<Expr> expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_shared<Grouping>(expr);
    }

    throw error(peek(), "Expect expression.");
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
                error(peek(), "Can't have more than 255 arguments.");
            }
            arguments.push_back(expression());
        } while (match({TokenType::COMMA}));
    }
    Token paren = consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");
    return std::make_shared<Call>(callee, paren, arguments);
}

std::shared_ptr<Stmt> Parser::function(const std::string& kind, bool is_public) {
    Token name = consume(TokenType::IDENTIFIER, "Expect " + kind + " name.");
    consume(TokenType::LEFT_PAREN, "Expect '(' after " + kind + " name.");
    std::vector<Param> parameters;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            if (parameters.size() >= 255) {
                error(peek(), "Can't have more than 255 parameters.");
            }
            Token param_name = consume(TokenType::IDENTIFIER, "Expect parameter name.");
            consume(TokenType::COLON, "Expect ':' after parameter name.");
            std::shared_ptr<Type> param_type = parseType();
            parameters.push_back({param_name, param_type});
        } while (match({TokenType::COMMA}));
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");
    consume(TokenType::LEFT_BRACE, "Expect '{' before " + kind + " body.");
    std::vector<std::shared_ptr<Stmt>> body = block();
    return std::make_shared<Func>(name, parameters, body, is_public);
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

std::shared_ptr<Stmt> Parser::structDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect struct name.");
    consume(TokenType::LEFT_BRACE, "Expect '{' before struct body.");
    std::vector<std::shared_ptr<Var>> fields;
    std::vector<std::shared_ptr<Func>> methods;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        bool is_public = true;
        if (match({TokenType::PRIVATE})) {
            is_public = false;
        } else if (match({TokenType::PUBLIC})) {
            is_public = true;
        }

        if (peek().type == TokenType::FUNC) {
            advance();
            methods.push_back(std::dynamic_pointer_cast<Func>(function("method", is_public)));
        }
        else {
            bool isMutable = false;
            if (match({TokenType::MUT})) {
                isMutable = true;
            } else if (!match({TokenType::LET})) {
                throw error(peek(), "Expect 'let' or 'mut' for field declaration.");
            }
            fields.push_back(std::dynamic_pointer_cast<Var>(varDeclaration(isMutable, is_public, true)));
        }
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after struct body.");
    return std::make_shared<Struct>(name, fields, methods);
}

std::shared_ptr<Type> Parser::parseType() {
    bool is_ref = false;
    bool is_mut_ref = false;

    if (match(std::vector<TokenType>{TokenType::AND})) {
        is_ref = true;
        if (match(std::vector<TokenType>{TokenType::MUT})) {
            is_mut_ref = true;
        }
    }

    std::shared_ptr<Type> type;
    if (match({TokenType::IDENTIFIER})) {
        Token type_name = previous();
        if (type_name.lexeme == "array") {
            consume(TokenType::LEFT_BRACKET, "Expect '[' after 'array'.");
            std::shared_ptr<Type> element_type = parseType();
            consume(TokenType::RIGHT_BRACKET, "Expect ']' after array element type.");
            type = std::make_shared<ArrayType>(element_type);
        } else {
            type = std::make_shared<PrimitiveType>(type_name.lexeme);
        }
    } else {
        throw error(peek(), "Expect type name.");
    }

    type->is_ref = is_ref;
    type->is_mut_ref = is_mut_ref;
    return type;
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
    std::cerr << "[line " << token.line << "] Error";
    if (token.type == TokenType::END_OF_FILE) {
        std::cerr << " at end";
    } else {
        std::cerr << " at '" << token.lexeme << "'";
    }
    std::cerr << ": " << message << std::endl;
    return ParseError();
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
            default:
                ;
        }
        advance();
    }
}
