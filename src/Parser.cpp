#include "Parser.h"
#include <stdexcept>
#include <sstream>

namespace Chtholly {

static std::map<TokenType, Precedence> precedences = {
    {TokenType::Equal, EQUALS},
    {TokenType::NotEqual, EQUALS},
    {TokenType::LessThan, LESSGREATER},
    {TokenType::GreaterThan, LESSGREATER},
    {TokenType::Plus, SUM},
    {TokenType::Minus, SUM},
    {TokenType::Slash, PRODUCT},
    {TokenType::Asterisk, PRODUCT},
    {TokenType::LParen, CALL},
};

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
        peekError(t);
        return false;
    }
}

void Parser::peekError(TokenType t) {
    std::stringstream ss;
    ss << "expected next token to be " << static_cast<int>(t) << ", got " << static_cast<int>(m_peekToken.type) << " instead";
    m_errors.push_back(ss.str());
}

Precedence Parser::peekPrecedence() {
    if (precedences.find(m_peekToken.type) != precedences.end()) {
        return precedences[m_peekToken.type];
    }
    return LOWEST;
}

Precedence Parser::curPrecedence() {
    if (precedences.find(m_curToken.type) != precedences.end()) {
        return precedences[m_curToken.type];
    }
    return LOWEST;
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
    case TokenType::Mut:
        return parseDeclarationStatement();
    case TokenType::Return:
        return parseReturnStatement();
    default:
        return parseExpressionStatement();
    }
}

std::unique_ptr<LetStatement> Parser::parseDeclarationStatement() {
    auto stmt = std::make_unique<LetStatement>(m_curToken);
    stmt->isMutable = m_curToken.type == TokenType::Mut;

    if (!expectPeek(TokenType::Identifier)) {
        return nullptr;
    }

    stmt->name = std::make_unique<Identifier>(m_curToken, m_curToken.literal);

    if (m_peekToken.type == TokenType::Colon) {
        nextToken();
        nextToken();
        stmt->type = parseType();
    }

    if (!expectPeek(TokenType::Assign)) {
        return nullptr;
    }

    nextToken();

    stmt->value = parseExpression(LOWEST);

    if (m_peekToken.type == TokenType::Semicolon) {
        nextToken();
    }

    return stmt;
}

std::unique_ptr<ReturnStatement> Parser::parseReturnStatement() {
    auto stmt = std::make_unique<ReturnStatement>(m_curToken);

    nextToken();

    stmt->returnValue = parseExpression(LOWEST);

    if (m_peekToken.type == TokenType::Semicolon) {
        nextToken();
    }

    return stmt;
}

std::unique_ptr<ExpressionStatement> Parser::parseExpressionStatement() {
    auto stmt = std::make_unique<ExpressionStatement>(m_curToken);

    stmt->expression = parseExpression(LOWEST);

    if (m_peekToken.type == TokenType::Semicolon) {
        nextToken();
    }

    return stmt;
}

std::unique_ptr<Expression> Parser::parseExpression(Precedence precedence) {
    std::unique_ptr<Expression> leftExp;
    switch (m_curToken.type) {
    case TokenType::Identifier:
        leftExp = std::make_unique<Identifier>(m_curToken, m_curToken.literal);
        break;
    case TokenType::Int:
        leftExp = std::make_unique<IntegerLiteral>(m_curToken, std::stoll(m_curToken.literal));
        break;
    case TokenType::String:
        leftExp = std::make_unique<StringLiteral>(m_curToken, m_curToken.literal);
        break;
    case TokenType::True:
    case TokenType::False:
        leftExp = std::make_unique<Boolean>(m_curToken, m_curToken.type == TokenType::True);
        break;
    case TokenType::Bang:
    case TokenType::Minus:
        leftExp = parsePrefixExpression();
        break;
    case TokenType::LParen:
        nextToken();
        leftExp = parseExpression(LOWEST);
        if (!expectPeek(TokenType::RParen)) {
            return nullptr;
        }
        break;
    case TokenType::If:
        leftExp = parseIfExpression();
        break;
    case TokenType::Func:
        leftExp = parseFunctionLiteral();
        break;
    default:
        return nullptr;
    }

    while (m_peekToken.type != TokenType::Semicolon && precedence < peekPrecedence()) {
        nextToken();
        if (m_curToken.type == TokenType::LParen) {
            leftExp = parseCallExpression(std::move(leftExp));
        } else {
            leftExp = parseInfixExpression(std::move(leftExp));
        }
    }

    return leftExp;
}

std::unique_ptr<Expression> Parser::parsePrefixExpression() {
    auto expression = std::make_unique<PrefixExpression>(m_curToken, m_curToken.literal);
    nextToken();
    expression->right = parseExpression(PREFIX);
    return expression;
}

std::unique_ptr<Expression> Parser::parseInfixExpression(std::unique_ptr<Expression> left) {
    auto expression = std::make_unique<InfixExpression>(m_curToken, m_curToken.literal, std::move(left));
    auto precedence = curPrecedence();
    nextToken();
    expression->right = parseExpression(precedence);
    return expression;
}

std::unique_ptr<BlockStatement> Parser::parseBlockStatement() {
    auto block = std::make_unique<BlockStatement>(m_curToken);

    nextToken();

    while (m_curToken.type != TokenType::RBrace && m_curToken.type != TokenType::Eof) {
        auto stmt = parseStatement();
        if (stmt) {
            block->statements.push_back(std::move(stmt));
        }
        nextToken();
    }

    return block;
}

std::unique_ptr<Expression> Parser::parseIfExpression() {
    auto expression = std::make_unique<IfExpression>(m_curToken);

    if (!expectPeek(TokenType::LParen)) {
        return nullptr;
    }

    nextToken();
    expression->condition = parseExpression(LOWEST);

    if (!expectPeek(TokenType::RParen)) {
        return nullptr;
    }

    if (!expectPeek(TokenType::LBrace)) {
        return nullptr;
    }

    expression->consequence = parseBlockStatement();

    if (m_peekToken.type == TokenType::Else) {
        nextToken();

        if (!expectPeek(TokenType::LBrace)) {
            return nullptr;
        }

        expression->alternative = parseBlockStatement();
    }

    return expression;
}

std::unique_ptr<Expression> Parser::parseFunctionLiteral() {
    auto literal = std::make_unique<FunctionLiteral>(m_curToken);

    if (!expectPeek(TokenType::LParen)) {
        return nullptr;
    }

    literal->parameters = parseFunctionParameters();

    if (m_peekToken.type == TokenType::Arrow) {
        nextToken();
        nextToken();
        literal->returnType = parseType();
    }

    if (!expectPeek(TokenType::LBrace)) {
        return nullptr;
    }

    literal->body = parseBlockStatement();

    return literal;
}

std::vector<std::unique_ptr<Identifier>> Parser::parseFunctionParameters() {
    std::vector<std::unique_ptr<Identifier>> identifiers;

    if (m_peekToken.type == TokenType::RParen) {
        nextToken();
        return identifiers;
    }

    nextToken();

    auto ident = std::make_unique<Identifier>(m_curToken, m_curToken.literal);
    if (m_peekToken.type == TokenType::Colon) {
        nextToken();
        nextToken();
        ident->type = parseType();
    }
    identifiers.push_back(std::move(ident));

    while (m_peekToken.type == TokenType::Comma) {
        nextToken();
        nextToken();
        auto ident = std::make_unique<Identifier>(m_curToken, m_curToken.literal);
        if (m_peekToken.type == TokenType::Colon) {
            nextToken();
            nextToken();
            ident->type = parseType();
        }
        identifiers.push_back(std::move(ident));
    }

    if (!expectPeek(TokenType::RParen)) {
        return {};
    }

    return identifiers;
}

std::unique_ptr<Type> Parser::parseType() {
    return std::make_unique<TypeName>(m_curToken, m_curToken.literal);
}

std::unique_ptr<Expression> Parser::parseCallExpression(std::unique_ptr<Expression> function) {
    auto exp = std::make_unique<CallExpression>(m_curToken, std::move(function));
    exp->arguments = parseCallArguments();
    return exp;
}

std::vector<std::unique_ptr<Expression>> Parser::parseCallArguments() {
    std::vector<std::unique_ptr<Expression>> args;

    if (m_peekToken.type == TokenType::RParen) {
        nextToken();
        return args;
    }

    nextToken();
    args.push_back(parseExpression(LOWEST));

    while (m_peekToken.type == TokenType::Comma) {
        nextToken();
        nextToken();
        args.push_back(parseExpression(LOWEST));
    }

    if (!expectPeek(TokenType::RParen)) {
        return {};
    }

    return args;
}

} // namespace Chtholly
