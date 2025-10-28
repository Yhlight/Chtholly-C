#include "Parser.h"
#include <stdexcept>

namespace Chtholly {

Parser::Parser(Lexer& lexer) : lexer(lexer) {
    nextToken();
    nextToken();

    registerPrefix(TokenType::Identifier, &Parser::parseIdentifier);
    registerPrefix(TokenType::Integer, &Parser::parseIntegerLiteral);

    registerInfix(TokenType::Plus, &Parser::parseInfixExpression);
    registerInfix(TokenType::Minus, &Parser::parseInfixExpression);
    registerInfix(TokenType::Slash, &Parser::parseInfixExpression);
    registerInfix(TokenType::Asterisk, &Parser::parseInfixExpression);
    registerInfix(TokenType::Equal, &Parser::parseInfixExpression);
    registerInfix(TokenType::NotEqual, &Parser::parseInfixExpression);
    registerInfix(TokenType::Less, &Parser::parseInfixExpression);
    registerInfix(TokenType::Greater, &Parser::parseInfixExpression);

    precedences = {
        {TokenType::Equal, Precedence::EQUALS},
        {TokenType::NotEqual, Precedence::EQUALS},
        {TokenType::Less, Precedence::LESSGREATER},
        {TokenType::Greater, Precedence::LESSGREATER},
        {TokenType::Plus, Precedence::SUM},
        {TokenType::Minus, Precedence::SUM},
        {TokenType::Slash, Precedence::PRODUCT},
        {TokenType::Asterisk, Precedence::PRODUCT},
    };
}

void Parser::registerPrefix(TokenType t, PrefixParseFn fn) {
    prefixParseFns[t] = fn;
}

void Parser::registerInfix(TokenType t, InfixParseFn fn) {
    infixParseFns[t] = fn;
}

Precedence Parser::peekPrecedence() {
    if (precedences.find(peekToken.type) != precedences.end()) {
        return precedences[peekToken.type];
    }
    return Precedence::LOWEST;
}

Precedence Parser::curPrecedence() {
    if (precedences.find(currentToken.type) != precedences.end()) {
        return precedences[currentToken.type];
    }
    return Precedence::LOWEST;
}

void Parser::nextToken() {
    currentToken = peekToken;
    peekToken = lexer.nextToken();
}

bool Parser::expectPeek(TokenType t) {
    if (peekToken.type == t) {
        nextToken();
        return true;
    }
    return false;
}

std::unique_ptr<Program> Parser::parseProgram() {
    auto program = std::make_unique<Program>();
    while (currentToken.type != TokenType::Eof) {
        auto stmt = parseStatement();
        if (stmt) {
            program->statements.push_back(std::move(stmt));
        }
        nextToken();
    }
    return program;
}

std::unique_ptr<Statement> Parser::parseStatement() {
    if (currentToken.type == TokenType::Let || currentToken.type == TokenType::Mut) {
        return parseVariableDeclaration();
    } else if (currentToken.type == TokenType::Return) {
        return parseReturnStatement();
    }
    return parseExpressionStatement();
}

std::unique_ptr<VariableDeclaration> Parser::parseVariableDeclaration() {
    Token varToken = currentToken;
    if (!expectPeek(TokenType::Identifier)) return nullptr;
    auto name = std::make_unique<Identifier>(currentToken);
    if (!expectPeek(TokenType::Assign)) return nullptr;
    nextToken();
    auto value = parseExpression(Precedence::LOWEST);
    if (peekToken.type == TokenType::Semicolon) nextToken();
    return std::make_unique<VariableDeclaration>(varToken, std::move(name), std::move(value));
}

std::unique_ptr<ReturnStatement> Parser::parseReturnStatement() {
    Token returnToken = currentToken;
    nextToken();
    auto returnValue = parseExpression(Precedence::LOWEST);
    if (peekToken.type == TokenType::Semicolon) nextToken();
    return std::make_unique<ReturnStatement>(returnToken, std::move(returnValue));
}

std::unique_ptr<ExpressionStatement> Parser::parseExpressionStatement() {
    auto expr = parseExpression(Precedence::LOWEST);
    auto stmt = std::make_unique<ExpressionStatement>(std::move(expr));
    if (peekToken.type == TokenType::Semicolon) nextToken();
    return stmt;
}

std::unique_ptr<Expression> Parser::parseExpression(Precedence precedence) {
    auto prefix = prefixParseFns[currentToken.type];
    if (prefix == nullptr) return nullptr;
    auto leftExp = (this->*prefix)();

    while (peekToken.type != TokenType::Semicolon && precedence < peekPrecedence()) {
        auto infix = infixParseFns[peekToken.type];
        if (infix == nullptr) return leftExp;
        nextToken();
        leftExp = (this->*infix)(std::move(leftExp));
    }
    return leftExp;
}

std::unique_ptr<Expression> Parser::parseIdentifier() {
    return std::make_unique<Identifier>(currentToken);
}

std::unique_ptr<Expression> Parser::parseIntegerLiteral() {
    long long value = std::stoll(currentToken.literal);
    return std::make_unique<IntegerLiteral>(currentToken, value);
}

std::unique_ptr<Expression> Parser::parseInfixExpression(std::unique_ptr<Expression> left) {
    Token op = currentToken;
    Precedence precedence = curPrecedence();
    nextToken();
    auto right = parseExpression(precedence);
    return std::make_unique<BinaryExpression>(std::move(left), op, std::move(right));
}

} // namespace Chtholly
