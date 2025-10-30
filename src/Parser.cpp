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
    return nullptr;
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

std::unique_ptr<Expr> Parser::expression() {
    return primary();
}

std::unique_ptr<Expr> Parser::primary() {
    if (match({TokenType::NUMBER_INT, TokenType::NUMBER_FLOAT})) {
        return std::make_unique<NumericLiteral>(tokens_[current_ - 1]);
    }
    // Other primary expressions will go here
    return nullptr;
}
