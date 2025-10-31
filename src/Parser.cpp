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
    if (match({TokenType::FUNC})) {
        return functionDeclaration();
    }
    if (match({TokenType::LET, TokenType::MUT})) {
        bool isMutable = tokens_[current_ - 1].type == TokenType::MUT;
        if (tokens_[current_ - 1].type == TokenType::LET) {
            return letDeclaration(false);
        }
        return letDeclaration(true);
    }
    return statement();
}

std::unique_ptr<Stmt> Parser::statement() {
    if (match({TokenType::RETURN})) {
        return returnStatement();
    }
    if (match({TokenType::LEFT_BRACE})) {
        return block();
    }

    auto expr = expression();
    if (dynamic_cast<CallExpr*>(expr.get()) || dynamic_cast<AssignExpr*>(expr.get())) {
        consume(TokenType::SEMICOLON, "Expect ';' after expression.");
        return std::make_unique<ExprStmt>(std::move(expr));
    }

    throw std::runtime_error("Expect a statement.");
}

std::unique_ptr<Stmt> Parser::letDeclaration(bool isMutable) {
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
    return std::make_unique<LetStmt>(name, std::move(type), std::move(initializer), isMutable);
}

std::unique_ptr<Type> Parser::parseType() {
    if (match({TokenType::AMPERSAND})) {
        bool isMutable = match({TokenType::MUT});
        std::unique_ptr<Type> referencedType = parseType();
        return std::make_unique<ReferenceType>(std::move(referencedType), isMutable);
    }

    Token typeToken = consume(TokenType::IDENTIFIER, "Expect type name.");
    if (typeToken.lexeme == "int") {
        return std::make_unique<PrimitiveType>(PrimitiveType::Kind::Int);
    } else if (typeToken.lexeme == "string") {
        return std::make_unique<PrimitiveType>(PrimitiveType::Kind::String);
    }
    // Add other types here
    return nullptr;
}

std::unique_ptr<Stmt> Parser::functionDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect function name.");
    consume(TokenType::LEFT_PAREN, "Expect '(' after function name.");
    std::vector<FuncStmt::Parameter> params;
    if (peek().type != TokenType::RIGHT_PAREN) {
        do {
            bool isMutable = match({TokenType::MUT});
            Token paramName = consume(TokenType::IDENTIFIER, "Expect parameter name.");
            consume(TokenType::COLON, "Expect ':' after parameter name.");
            std::unique_ptr<Type> paramType = parseType();
            params.push_back({paramName, std::move(paramType), isMutable});
        } while (match({TokenType::COMMA}));
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");

    std::unique_ptr<Type> returnType = nullptr;
    if (match({TokenType::ARROW})) {
        returnType = parseType();
    }

    consume(TokenType::LEFT_BRACE, "Expect '{' before function body.");
    std::unique_ptr<BlockStmt> body = block();

    return std::make_unique<FuncStmt>(name, std::move(params), std::move(returnType), std::move(body));
}

std::unique_ptr<BlockStmt> Parser::block() {
    std::vector<std::unique_ptr<Stmt>> statements;
    while (peek().type != TokenType::RIGHT_BRACE && !isAtEnd()) {
        statements.push_back(declaration());
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
    return std::make_unique<BlockStmt>(std::move(statements));
}

std::unique_ptr<Stmt> Parser::returnStatement() {
    Token keyword = tokens_[current_ - 1];
    std::unique_ptr<Expr> value = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after return value.");
    return std::make_unique<ReturnStmt>(keyword, std::move(value));
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
    } else if (match({TokenType::STRING})) {
        return std::make_unique<StringLiteral>(tokens_[current_ - 1]);
    } else if (match({TokenType::MINUS, TokenType::BANG})) {
        Token op = tokens_[current_ - 1];
        std::unique_ptr<Expr> right = parsePrecedence(6); // Unary precedence
        return std::make_unique<UnaryExpr>(op, std::move(right));
    } else if (match({TokenType::AMPERSAND})) {
        bool isMutable = match({TokenType::MUT});
        std::unique_ptr<Expr> expr = parsePrecedence(6); // Unary precedence
        return std::make_unique<BorrowExpr>(std::move(expr), isMutable);
    } else if (match({TokenType::LEFT_PAREN})) {
        std::unique_ptr<Expr> expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_unique<GroupingExpr>(std::move(expr));
    } else if (match({TokenType::IDENTIFIER})) {
        return std::make_unique<VariableExpr>(tokens_[current_ - 1]);
    }
    return nullptr;
}

std::unique_ptr<Expr> Parser::infix(std::unique_ptr<Expr> left) {
    if (peek().type == TokenType::EQUAL) {
        advance();
        if (auto varExpr = dynamic_cast<VariableExpr*>(left.get())) {
            std::unique_ptr<Expr> value = parsePrecedence(getPrecedence(TokenType::EQUAL) - 1);
            return std::make_unique<AssignExpr>(varExpr->name, std::move(value));
        }
        throw std::runtime_error("Invalid assignment target.");
    }

    if (peek().type == TokenType::LEFT_PAREN) {
        advance();
        std::vector<std::unique_ptr<Expr>> args;
        if (peek().type != TokenType::RIGHT_PAREN) {
            do {
                args.push_back(expression());
            } while (match({TokenType::COMMA}));
        }
        Token paren = consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");
        return std::make_unique<CallExpr>(std::move(left), paren, std::move(args));
    }

    Token op = advance();
    int precedence = getPrecedence(op.type);
    std::unique_ptr<Expr> right = parsePrecedence(precedence);
    return std::make_unique<BinaryExpr>(std::move(left), op, std::move(right));
}

int Parser::getPrecedence(TokenType type) {
    switch (type) {
        case TokenType::EQUAL:
            return 1;
        case TokenType::LOGICAL_OR:
            return 2;
        case TokenType::LOGICAL_AND:
            return 3;
        case TokenType::EQUAL_EQUAL:
        case TokenType::BANG_EQUAL:
        case TokenType::LESS:
        case TokenType::LESS_EQUAL:
        case TokenType::GREATER:
        case TokenType::GREATER_EQUAL:
            return 4;
        case TokenType::PLUS:
        case TokenType::MINUS:
            return 5;
        case TokenType::STAR:
        case TokenType::SLASH:
        case TokenType::MODULO:
            return 6;
        case TokenType::LEFT_PAREN:
            return 7;
        default:
            return 0;
    }
}
