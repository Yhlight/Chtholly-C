#include "Parser.h"
#include <stdexcept>

namespace Chtholly {

Parser::Parser(Lexer& lexer) : m_lexer(lexer) {
    // Read two tokens, so m_curToken and m_peekToken are both set
    nextToken();
    nextToken();
}

void Parser::nextToken() {
    m_curToken = m_peekToken;
    m_peekToken = m_lexer.nextToken();
}

bool Parser::expectPeek(TokenType t) {
    if (m_peekToken.type == t) {
        nextToken();
        return true;
    } else {
        return false;
    }
}

std::unique_ptr<Program> Parser::parseProgram() {
    auto program = std::make_unique<Program>();

    while (m_curToken.type != TokenType::Eof) {
        auto stmt = parseStatement();
        if (stmt) {
            program->statements.push_back(std::move(stmt));
        }
        nextToken();
    }

    return program;
}

std::unique_ptr<Statement> Parser::parseStatement() {
    switch (m_curToken.type) {
    case TokenType::Let:
        return parseLetStatement();
    case TokenType::Return:
        return parseReturnStatement();
    default:
        return parseExpressionStatement();
    }
}

std::unique_ptr<LetStatement> Parser::parseLetStatement() {
    auto stmt = std::make_unique<LetStatement>(m_curToken);

    if (!expectPeek(TokenType::Identifier)) {
        return nullptr;
    }

    stmt->name = std::make_unique<Identifier>(m_curToken, m_curToken.literal);

    if (!expectPeek(TokenType::Assign)) {
        return nullptr;
    }

    nextToken();

    stmt->value = parseExpression();

    if (m_peekToken.type == TokenType::Semicolon) {
        nextToken();
    }

    return stmt;
}

std::unique_ptr<ReturnStatement> Parser::parseReturnStatement() {
    auto stmt = std::make_unique<ReturnStatement>(m_curToken);

    nextToken();

    stmt->returnValue = parseExpression();

    if (m_peekToken.type == TokenType::Semicolon) {
        nextToken();
    }

    return stmt;
}

std::unique_ptr<ExpressionStatement> Parser::parseExpressionStatement() {
    auto stmt = std::make_unique<ExpressionStatement>(m_curToken);

    stmt->expression = parseExpression();

    if (m_peekToken.type == TokenType::Semicolon) {
        nextToken();
    }

    return stmt;
}

std::unique_ptr<Expression> Parser::parseExpression() {
    auto left = parseTerm();

    while (m_peekToken.type == TokenType::Plus || m_peekToken.type == TokenType::Minus) {
        nextToken();
        auto op = m_curToken.literal;
        nextToken();
        auto right = parseTerm();
        auto infix = std::make_unique<InfixExpression>(m_curToken, op, std::move(left));
        infix->right = std::move(right);
        left = std::move(infix);
    }

    return left;
}

std::unique_ptr<Expression> Parser::parseTerm() {
    auto left = parseFactor();

    while (m_peekToken.type == TokenType::Asterisk || m_peekToken.type == TokenType::Slash) {
        nextToken();
        auto op = m_curToken.literal;
        nextToken();
        auto right = parseFactor();
        auto infix = std::make_unique<InfixExpression>(m_curToken, op, std::move(left));
        infix->right = std::move(right);
        left = std::move(infix);
    }

    return left;
}

std::unique_ptr<Expression> Parser::parseFactor() {
    if (m_curToken.type == TokenType::Int) {
        return std::make_unique<IntegerLiteral>(m_curToken, std::stoll(m_curToken.literal));
    }
    if (m_curToken.type == TokenType::Identifier) {
        return std::make_unique<Identifier>(m_curToken, m_curToken.literal);
    }
    if (m_curToken.type == TokenType::Bang || m_curToken.type == TokenType::Minus) {
        auto op = m_curToken.literal;
        nextToken();
        auto prefix = std::make_unique<PrefixExpression>(m_curToken, op);
        prefix->right = parseFactor();
        return prefix;
    }
    if (m_curToken.type == TokenType::LParen) {
        nextToken();
        auto exp = parseExpression();
        if (!expectPeek(TokenType::RParen)) {
            return nullptr;
        }
        return exp;
    }
    return nullptr;
}

} // namespace Chtholly
