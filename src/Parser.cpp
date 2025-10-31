#include "Parser.h"
#include <stdexcept>

namespace chtholly {

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

std::unique_ptr<BlockStmtAST> Parser::parse() {
    std::vector<std::unique_ptr<StmtAST>> statements;
    while (!isAtEnd()) {
        statements.push_back(parseStatement());
    }
    return std::make_unique<BlockStmtAST>(std::move(statements));
}

std::unique_ptr<StmtAST> Parser::parseStatement() {
    if (match({TokenType::If})) {
        return parseIfStmt();
    }
    if (match({TokenType::Switch})) {
        return parseSwitchStmt();
    }
    if (match({TokenType::Func})) {
        return parseFuncDecl();
    }
    if (match({TokenType::Let, TokenType::Mut})) {
        return parseVarDecl();
    }
    // Other statements will be handled here
    return nullptr;
}

std::unique_ptr<FuncDeclAST> Parser::parseFuncDecl() {
    Token name = consume(TokenType::Identifier, "Expect function name.");
    consume(TokenType::LeftParen, "Expect '(' after function name.");

    std::vector<Param> params;
    if (peek().type != TokenType::RightParen) {
        do {
            Token paramName = consume(TokenType::Identifier, "Expect parameter name.");
            consume(TokenType::Colon, "Expect ':' after parameter name.");
            Token paramType = consume(TokenType::Identifier, "Expect parameter type.");
            params.push_back({paramName.lexeme, paramType.lexeme});
        } while (match({TokenType::Comma}));
    }
    consume(TokenType::RightParen, "Expect ')' after parameters.");

    std::string returnTypeName = "void"; // Default return type
    if (match({TokenType::Arrow})) {
        returnTypeName = consume(TokenType::Identifier, "Expect return type.").lexeme;
    }

    consume(TokenType::LeftBrace, "Expect '{' before function body.");
    auto body = parseBlock();

    return std::make_unique<FuncDeclAST>(name.lexeme, std::move(params), returnTypeName, std::move(body));
}

std::unique_ptr<StmtAST> Parser::parseIfStmt() {
    consume(TokenType::LeftParen, "Expect '(' after 'if'.");
    auto condition = parseExpression();
    consume(TokenType::RightParen, "Expect ')' after if condition.");

    consume(TokenType::LeftBrace, "Expect '{' before then branch.");
    auto thenBranch = parseBlock();

    std::unique_ptr<BlockStmtAST> elseBranch = nullptr;
    if (match({TokenType::Else})) {
        consume(TokenType::LeftBrace, "Expect '{' before else branch.");
        elseBranch = parseBlock();
    }

    return std::make_unique<IfStmtAST>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

std::unique_ptr<StmtAST> Parser::parseSwitchStmt() {
    consume(TokenType::LeftParen, "Expect '(' after 'switch'.");
    auto expr = parseExpression();
    consume(TokenType::RightParen, "Expect ')' after switch expression.");
    consume(TokenType::LeftBrace, "Expect '{' before switch body.");

    std::vector<std::unique_ptr<CaseBlockAST>> cases;
    while (peek().type == TokenType::Case || peek().type == TokenType::Default) {
        if (match({TokenType::Case})) {
            auto caseExpr = parseExpression();
            consume(TokenType::Colon, "Expect ':' after case expression.");
            consume(TokenType::LeftBrace, "Expect '{' before case body.");
            auto caseBody = parseBlock();
            cases.push_back(std::make_unique<CaseBlockAST>(std::move(caseExpr), std::move(caseBody)));
        } else if (match({TokenType::Default})) {
            consume(TokenType::Colon, "Expect ':' after 'default'.");
            consume(TokenType::LeftBrace, "Expect '{' before default body.");
            auto caseBody = parseBlock();
            cases.push_back(std::make_unique<CaseBlockAST>(nullptr, std::move(caseBody)));
        }
    }

    consume(TokenType::RightBrace, "Expect '}' after switch body.");
    return std::make_unique<SwitchStmtAST>(std::move(expr), std::move(cases));
}

std::unique_ptr<BlockStmtAST> Parser::parseBlock() {
    std::vector<std::unique_ptr<StmtAST>> statements;
    while (peek().type != TokenType::RightBrace && !isAtEnd()) {
        statements.push_back(parseStatement());
    }
    consume(TokenType::RightBrace, "Expect '}' after block.");
    return std::make_unique<BlockStmtAST>(std::move(statements));
}

std::unique_ptr<StmtAST> Parser::parseVarDecl() {
    bool isMutable = peek(-1).type == TokenType::Mut;
    Token name = consume(TokenType::Identifier, "Expect variable name.");

    std::unique_ptr<ExprAST> initializer = nullptr;
    if (match({TokenType::Equal})) {
        initializer = parseExpression();
    }

    consume(TokenType::Semicolon, "Expect ';' after variable declaration.");
    return std::make_unique<VarDeclAST>(name.lexeme, std::move(initializer), isMutable);
}

std::unique_ptr<ExprAST> Parser::parseExpression() {
    return parseAssignment();
}

std::unique_ptr<ExprAST> Parser::parseAssignment() {
    auto expr = parseTerm();
    // Assignment logic will be here
    return expr;
}

std::unique_ptr<ExprAST> Parser::parseTerm() {
    auto expr = parseFactor();
    while (match({TokenType::Plus, TokenType::Minus})) {
        Token op = tokens[current - 1];
        auto right = parseFactor();
        expr = std::make_unique<BinaryExprAST>(op.type, std::move(expr), std::move(right));
    }
    return expr;
}

std::unique_ptr<ExprAST> Parser::parseFactor() {
    auto expr = parseUnary();
    while (match({TokenType::Star, TokenType::Slash})) {
        Token op = tokens[current - 1];
        auto right = parseUnary();
        expr = std::make_unique<BinaryExprAST>(op.type, std::move(expr), std::move(right));
    }
    return expr;
}

std::unique_ptr<ExprAST> Parser::parseUnary() {
    // Unary logic will be here
    return parseCall();
}

std::unique_ptr<ExprAST> Parser::parseCall() {
    auto expr = parsePrimary();

    if (match({TokenType::LeftParen})) {
        std::vector<std::unique_ptr<ExprAST>> args;
        if (peek().type != TokenType::RightParen) {
            do {
                args.push_back(parseExpression());
            } while (match({TokenType::Comma}));
        }
        consume(TokenType::RightParen, "Expect ')' after arguments.");
        return std::make_unique<CallExprAST>(std::move(expr), std::move(args));
    }

    return expr;
}


std::unique_ptr<ExprAST> Parser::parsePrimary() {
    if (match({TokenType::Integer})) {
        return std::make_unique<NumberExprAST>(std::get<int>(tokens[current - 1].literal));
    }
    if (match({TokenType::Double})) {
        return std::make_unique<NumberExprAST>(std::get<double>(tokens[current - 1].literal));
    }
    if (match({TokenType::String})) {
        return std::make_unique<StringExprAST>(std::get<std::string>(tokens[current - 1].literal));
    }
    if (match({TokenType::Identifier})) {
        return std::make_unique<VariableExprAST>(tokens[current - 1].lexeme);
    }
    if (match({TokenType::True})) {
        return std::make_unique<BoolExprAST>(true);
    }
    if (match({TokenType::False})) {
        return std::make_unique<BoolExprAST>(false);
    }
    // Other primary expressions will be here
    return nullptr;
}

Token& Parser::peek(int k) {
    if (current + k >= tokens.size()) {
        return tokens.back();
    }
    return tokens[current + k];
}

Token& Parser::advance() {
    if (!isAtEnd()) current++;
    return tokens[current - 1];
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

Token& Parser::consume(TokenType type, const std::string& message) {
    if (peek().type == type) return advance();
    throw std::runtime_error(message);
}

bool Parser::isAtEnd() {
    return peek().type == TokenType::EndOfFile;
}

} // namespace chtholly
