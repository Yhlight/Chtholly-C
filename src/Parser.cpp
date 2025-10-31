#include "Parser.h"

Parser::Parser(Lexer& lexer) : lexer(lexer) {
    next_token();
    next_token();
}

std::unique_ptr<Program> Parser::parse_program() {
    auto program = std::make_unique<Program>();

    while (current_token.type != TokenType::Eof) {
        auto stmt = parse_statement();
        if (stmt) {
            program->statements.push_back(std::move(stmt));
        }
        next_token();
    }

    return program;
}

const std::vector<std::string>& Parser::errors() const {
    return error_messages;
}

void Parser::next_token() {
    current_token = peek_token;
    peek_token = lexer.next_token();
}

std::unique_ptr<Statement> Parser::parse_statement() {
    if (current_token.type == TokenType::Let) {
        return parse_let_statement();
    }
    return nullptr;
}

std::unique_ptr<Expression> parse_expression() {
    // For now, we'll just return a placeholder.
    return std::make_unique<Identifier>("EXPRESSION");
}

std::unique_ptr<LetStatement> Parser::parse_let_statement() {
    auto stmt = std::make_unique<LetStatement>();

    next_token(); // consume 'let'

    if (current_token.type != TokenType::Identifier) {
        error_messages.push_back("Expected identifier after 'let'");
        return nullptr;
    }

    stmt->name = std::make_unique<Identifier>(current_token.literal);

    next_token(); // consume identifier

    if (current_token.type != TokenType::Equal) {
        error_messages.push_back("Expected '=' after identifier");
        return nullptr;
    }

    next_token(); // consume '='

    stmt->value = parse_expression();

    while (current_token.type != TokenType::Semicolon && current_token.type != TokenType::Eof) {
        next_token();
    }

    return stmt;
}
