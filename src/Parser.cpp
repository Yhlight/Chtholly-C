#include "Parser.h"
#include <stdexcept>

Parser::Parser(Lexer& lexer) : lexer_(lexer) {
    Token token;
    do {
        token = lexer_.nextToken();
        tokens_.push_back(token);
    } while (token.type != TokenType::END_OF_FILE);
}

std::vector<std::unique_ptr<Stmt>> Parser::parse() {
    std::vector<std::unique_ptr<Stmt>> statements;
    while (!isAtEnd()) {
        statements.push_back(declaration());
    }
    return statements;
}

Token Parser::advance() {
    if (!isAtEnd()) {
        current_++;
    }
    return tokens_[current_ - 1];
}

Token Parser::peek(int k) const {
    if (current_ + k >= tokens_.size()) {
        return tokens_.back();
    }
    return tokens_[current_ + k];
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::END_OF_FILE;
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

Token Parser::consume(TokenType type, const std::string& message) {
    if (peek().type == type) {
        return advance();
    }
    throw std::runtime_error(message);
}

std::unique_ptr<Stmt> Parser::declaration() {
    if (match({TokenType::LET})) {
        return letDeclaration();
    }
    // Other statement types will go here

    throw std::runtime_error("Expect a declaration.");
}

std::unique_ptr<Stmt> Parser::letDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");

    std::unique_ptr<Type> type = nullptr;
    if (match({TokenType::COLON})) {
        type = parseType();
    }

    std::unique_ptr<Expr> initializer = nullptr;
    if (match({TokenType::EQUAL})) {
        initializer = expression();
    }

    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_unique<LetStmt>(name, std::move(type), std::move(initializer));
}

std::unique_ptr<Type> Parser::parseType() {
    Token typeToken = consume(TokenType::IDENTIFIER, "Expect type name.");
    if (typeToken.lexeme == "int") {
        return std::make_unique<PrimitiveType>(PrimitiveType::Kind::Int);
    } else if (typeToken.lexeme == "string") {
        return std::make_unique<PrimitiveType>(PrimitiveType::Kind::String);
    }
    // Add other types here
    return nullptr;
}

std::unique_ptr<Expr> Parser::expression() {
    return parsePrecedence(0);
}

std::unique_ptr<Expr> Parser::parsePrecedence(int precedence) {
    std::unique_ptr<Expr> left = prefix();

    while (precedence < getPrecedence(peek().type)) {
        left = infix(std::move(left));
    }

    return left;
}

std::unique_ptr<Expr> Parser::prefix() {
    if (match({TokenType::NUMBER_INT, TokenType::NUMBER_FLOAT})) {
        return std::make_unique<NumericLiteral>(tokens_[current_ - 1]);
    } else if (match({TokenType::MINUS, TokenType::BANG})) {
        Token op = tokens_[current_ - 1];
        std::unique_ptr<Expr> right = parsePrecedence(6); // Unary precedence
        return std::make_unique<UnaryExpr>(op, std::move(right));
    } else if (match({TokenType::LEFT_PAREN})) {
        std::unique_ptr<Expr> expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_unique<GroupingExpr>(std::move(expr));
    }
    return nullptr;
}

std::unique_ptr<Expr> Parser::infix(std::unique_ptr<Expr> left) {
    Token op = advance();
    int precedence = getPrecedence(op.type);
    std::unique_ptr<Expr> right = parsePrecedence(precedence);
    return std::make_unique<BinaryExpr>(std::move(left), op, std::move(right));
}

int Parser::getPrecedence(TokenType type) {
    switch (type) {
        case TokenType::PLUS:
        case TokenType::MINUS:
            return 1;
        case TokenType::STAR:
        case TokenType::SLASH:
        case TokenType::MODULO:
            return 2;
        case TokenType::EQUAL_EQUAL:
        case TokenType::BANG_EQUAL:
        case TokenType::LESS:
        case TokenType::LESS_EQUAL:
        case TokenType::GREATER:
        case TokenType::GREATER_EQUAL:
            return 3;
        case TokenType::LOGICAL_AND:
            return 4;
        case TokenType::LOGICAL_OR:
            return 5;
        default:
            return 0;
    }
}
