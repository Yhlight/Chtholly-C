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
    if (match({TokenType::Enum})) {
        return parseEnumDecl();
    }
    if (match({TokenType::While})) {
        return parseWhileStmt();
    }
    if (match({TokenType::For})) {
        return parseForStmt();
    }
    if (match({TokenType::Import})) {
        return parseImportStmt();
    }
    if (match({TokenType::Struct})) {
        return parseStructDecl();
    }
    if (match({TokenType::Func})) {
        return parseFuncDecl();
    }
    if (match({TokenType::Let, TokenType::Mut})) {
        return parseVarDecl();
    }

    auto expr = parseExpression();
    consume(TokenType::Semicolon, "Expect ';' after expression.");
    return std::make_unique<ExprStmtAST>(std::move(expr));
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

std::unique_ptr<StmtAST> Parser::parseEnumDecl() {
    Token name = consume(TokenType::Identifier, "Expect enum name.");
    consume(TokenType::LeftBrace, "Expect '{' before enum body.");

    std::vector<std::string> members;
    if (peek().type != TokenType::RightBrace) {
        do {
            members.push_back(consume(TokenType::Identifier, "Expect enum member name.").lexeme);
        } while (match({TokenType::Comma}));
    }

    consume(TokenType::RightBrace, "Expect '}' after enum body.");
    consume(TokenType::Semicolon, "Expect ';' after enum declaration.");
    return std::make_unique<EnumDeclAST>(name.lexeme, std::move(members));
}

std::unique_ptr<StmtAST> Parser::parseWhileStmt() {
    consume(TokenType::LeftParen, "Expect '(' after 'while'.");
    auto condition = parseExpression();
    consume(TokenType::RightParen, "Expect ')' after while condition.");

    consume(TokenType::LeftBrace, "Expect '{' before while body.");
    auto body = parseBlock();

    return std::make_unique<WhileStmtAST>(std::move(condition), std::move(body));
}

std::unique_ptr<StmtAST> Parser::parseForStmt() {
    consume(TokenType::LeftParen, "Expect '(' after 'for'.");

    std::unique_ptr<StmtAST> init = nullptr;
    if (peek().type != TokenType::Semicolon) {
        init = parseStatement();
    }

    std::unique_ptr<ExprAST> cond = nullptr;
    if (peek().type != TokenType::Semicolon) {
        cond = parseExpression();
    }
    consume(TokenType::Semicolon, "Expect ';' after loop condition.");

    std::unique_ptr<ExprAST> inc = nullptr;
    if (peek().type != TokenType::RightParen) {
        inc = parseExpression();
    }
    consume(TokenType::RightParen, "Expect ')' after for clauses.");

    consume(TokenType::LeftBrace, "Expect '{' before for body.");
    auto body = parseBlock();

    return std::make_unique<ForStmtAST>(std::move(init), std::move(cond), std::move(inc), std::move(body));
}

std::unique_ptr<StmtAST> Parser::parseImportStmt() {
    Token moduleName = consume(TokenType::Identifier, "Expect module name.");
    consume(TokenType::Semicolon, "Expect ';' after module name.");
    return std::make_unique<ImportStmtAST>(moduleName.lexeme);
}

std::unique_ptr<StmtAST> Parser::parseStructDecl() {
    Token name = consume(TokenType::Identifier, "Expect struct name.");
    consume(TokenType::LeftBrace, "Expect '{' before struct body.");

    std::vector<MemberVar> members;
    while (peek().type != TokenType::RightBrace) {
        bool isPublic = true;
        if (peek().type == TokenType::Public || peek().type == TokenType::Private) {
            isPublic = advance().type == TokenType::Public;
        }

        Token memberName = consume(TokenType::Identifier, "Expect member name.");
        consume(TokenType::Colon, "Expect ':' after member name.");
        Token typeName = consume(TokenType::Identifier, "Expect member type.");
        members.push_back({memberName.lexeme, typeName.lexeme, isPublic});

        if (!match({TokenType::Comma})) {
            break;
        }
    }

    consume(TokenType::RightBrace, "Expect '}' after struct body.");
    return std::make_unique<StructDeclAST>(name.lexeme, std::move(members));
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
    auto expr = parseComparison();
    if (match({TokenType::Equal})) {
        auto value = parseAssignment();
        if (auto* varExpr = dynamic_cast<VariableExprAST*>(expr.get())) {
            return std::make_unique<AssignExprAST>(std::move(expr), std::move(value));
        }
        // Error: Invalid assignment target
    }
    return expr;
}

std::unique_ptr<ExprAST> Parser::parseComparison() {
    auto expr = parseTerm();
    while (match({TokenType::Greater, TokenType::GreaterEqual, TokenType::Less, TokenType::LessEqual})) {
        Token op = tokens[current - 1];
        auto right = parseTerm();
        expr = std::make_unique<BinaryExprAST>(op.type, std::move(expr), std::move(right));
    }
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
