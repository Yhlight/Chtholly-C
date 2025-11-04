#include "PrattParser.h"
#include "Error.h"
#include "AST/Stmt.h"
#include <stdexcept>
#include <vector>

PrattParser::PrattParser(const std::vector<Token>& tokens) : tokens(tokens) {}

std::vector<std::unique_ptr<Stmt>> PrattParser::parse() {
    std::vector<std::unique_ptr<Stmt>> statements;
    while (!isAtEnd()) {
        auto decl = declaration();
        if (decl) {
            statements.push_back(std::move(decl));
        }
    }
    return statements;
}

std::unique_ptr<Stmt> PrattParser::declaration() {
    try {
        if (match(TokenType::FUNC)) return function("function");
        if (match(TokenType::STRUCT)) return structDeclaration();
        if (match(TokenType::TRAIT)) return traitDeclaration();
        if (match(TokenType::IMPL)) return implDeclaration();
        if (match(TokenType::IMPORT)) return importStatement();
        if (match(TokenType::ENUM)) return enumDeclaration();
        if (match(TokenType::LET)) return letDeclaration();
        return statement();
    } catch (const std::runtime_error& e) {
        synchronize();
        return nullptr;
    }
}

std::unique_ptr<Stmt> PrattParser::letDeclaration() {
    bool isMutable = match(TokenType::MUT);
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");

    std::optional<TypeInfo> type;
    if (match(TokenType::COLON)) {
        type = parseType();
    }

    std::unique_ptr<Expr> initializer = nullptr;
    if (match(TokenType::EQUAL)) {
        initializer = expression();
    }

    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_unique<LetStmt>(name, std::move(type), std::move(initializer), isMutable, true);
}

std::unique_ptr<Stmt> PrattParser::statement() {
    if (match(TokenType::IF)) return ifStatement();
    if (match(TokenType::WHILE)) return whileStatement();
    if (match(TokenType::RETURN)) return returnStatement();
    if (match(TokenType::LEFT_BRACE)) return std::make_unique<BlockStmt>(block());
    return expressionStatement();
}

std::unique_ptr<Stmt> PrattParser::ifStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
    std::unique_ptr<Expr> condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");

    std::unique_ptr<Stmt> thenBranch = statement();
    std::unique_ptr<Stmt> elseBranch = nullptr;
    if (match(TokenType::ELSE)) {
        elseBranch = statement();
    }

    return std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

std::unique_ptr<Stmt> PrattParser::whileStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
    std::unique_ptr<Expr> condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after while condition.");
    std::unique_ptr<Stmt> body = statement();

    return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
}

std::unique_ptr<Stmt> PrattParser::returnStatement() {
    Token keyword = previous();
    std::unique_ptr<Expr> value = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        value = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after return value.");
    return std::make_unique<ReturnStmt>(keyword, std::move(value));
}

std::vector<std::unique_ptr<Stmt>> PrattParser::block() {
    std::vector<std::unique_ptr<Stmt>> statements;

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        statements.push_back(declaration());
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
    return statements;
}

std::unique_ptr<Stmt> PrattParser::expressionStatement() {
    std::unique_ptr<Expr> expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    return std::make_unique<ExpressionStmt>(std::move(expr));
}


std::unique_ptr<Expr> PrattParser::expression() {
    std::unique_ptr<Expr> left = prefix();

    while (precedence() > 0) {
        left = infix(std::move(left));
    }

    return left;
}

int PrattParser::precedence() {
    switch (peek().type) {
        case TokenType::PLUS:
        case TokenType::MINUS:
            return 1;
        case TokenType::STAR:
        case TokenType::SLASH:
            return 2;
        case TokenType::LEFT_PAREN:
        case TokenType::COLON_COLON:
             return 3;
        default:
            return 0;
    }
}

std::unique_ptr<Expr> PrattParser::prefix() {
    Token token = advance();
    switch (token.type) {
        case TokenType::NUMBER:
            return std::make_unique<LiteralExpr>(std::get<double>(token.literal));
        case TokenType::STRING:
            return std::make_unique<LiteralExpr>(std::get<std::string>(token.literal));
        case TokenType::TRUE:
            return std::make_unique<LiteralExpr>(true);
        case TokenType::FALSE:
            return std::make_unique<LiteralExpr>(false);
        case TokenType::IDENTIFIER:
            return std::make_unique<VariableExpr>(token);
        case TokenType::LEFT_PAREN: {
            std::unique_ptr<Expr> expr = expression();
            consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
            return std::make_unique<GroupingExpr>(std::move(expr));
        }
        case TokenType::MINUS: {
            return std::make_unique<UnaryExpr>(token, prefix());
        }
        default:
            ErrorReporter::error(token.line, "Expect expression.");
            throw std::runtime_error("Expect expression.");
    }
}

std::unique_ptr<Expr> PrattParser::infix(std::unique_ptr<Expr> left) {
    if (peek().type == TokenType::LEFT_PAREN) {
        advance();
        std::vector<std::unique_ptr<Expr>> arguments;
        if (!check(TokenType::RIGHT_PAREN)) {
            do {
                arguments.push_back(expression());
            } while (match(TokenType::COMMA));
        }
        Token paren = consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");
        std::vector<TypeInfo> generic_args;
        return std::make_unique<CallExpr>(std::move(left), paren, std::move(arguments), std::move(generic_args));
    }

    if (peek().type == TokenType::COLON_COLON) {
        advance();
        Token name = consume(TokenType::IDENTIFIER, "Expect identifier after '::'.");
        return std::make_unique<GetExpr>(std::move(left), name);
    }

    Token op = advance();
    std::unique_ptr<Expr> right = expression();
    return std::make_unique<BinaryExpr>(std::move(left), op, std::move(right));
}

const Token& PrattParser::peek() const {
    return tokens[current];
}

const Token& PrattParser::previous() const {
    return tokens[current - 1];
}

bool PrattParser::isAtEnd() const {
    return peek().type == TokenType::END_OF_FILE;
}

Token PrattParser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool PrattParser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

template<typename... Types>
bool PrattParser::match(Types... types) {
    for (TokenType type : {types...}) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

Token PrattParser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    ErrorReporter::error(peek().line, message);
    throw std::runtime_error(message);
}

void PrattParser::synchronize() {
    advance();

    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return;

        switch (peek().type) {
            case TokenType::FUNC:
            case TokenType::LET:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::RETURN:
            case TokenType::IMPORT:
                return;
        }

        advance();
    }
}

TypeInfo PrattParser::parseType() {
    bool isRef = match(TokenType::AMPERSAND);
    bool isMut = false;
    if (isRef) {
        isMut = match(TokenType::MUT);
    }

    Token typeToken = consume(TokenType::IDENTIFIER, "Expect type name.");
    return TypeInfo(typeToken, isRef, isMut);
}

std::unique_ptr<Stmt> PrattParser::function(const std::string& kind, bool body_required) {
    Token name = consume(TokenType::IDENTIFIER, "Expect " + kind + " name.");
    consume(TokenType::LEFT_PAREN, "Expect '(' after " + kind + " name.");
    std::vector<std::pair<Token, TypeInfo>> parameters;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            Token paramName = consume(TokenType::IDENTIFIER, "Expect parameter name.");
            consume(TokenType::COLON, "Expect ':' after parameter name.");
            parameters.emplace_back(paramName, parseType());
        } while (match(TokenType::COMMA));
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");

    std::optional<TypeInfo> returnType;
    if (match(TokenType::ARROW)) {
        returnType = parseType();
    }

    if (body_required) {
        consume(TokenType::LEFT_BRACE, "Expect '{' before " + kind + " body.");
        std::vector<std::unique_ptr<Stmt>> body = block();
        return std::make_unique<FunctionStmt>(name, std::vector<Token>(), std::move(parameters), std::move(body), std::move(returnType));
    } else {
        consume(TokenType::SEMICOLON, "Expect ';' after " + kind + " signature.");
        return std::make_unique<FunctionStmt>(name, std::vector<Token>(), std::move(parameters), std::vector<std::unique_ptr<Stmt>>(), std::move(returnType));
    }
}

std::unique_ptr<Stmt> PrattParser::structDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect struct name.");
    consume(TokenType::LEFT_BRACE, "Expect '{' before struct body.");
    std::vector<std::unique_ptr<LetStmt>> fields;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        fields.push_back(std::unique_ptr<LetStmt>(dynamic_cast<LetStmt*>(letDeclaration().release())));
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after struct body.");
    return std::make_unique<StructStmt>(name, std::move(fields));
}

std::unique_ptr<Stmt> PrattParser::traitDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect trait name.");
    consume(TokenType::LEFT_BRACE, "Expect '{' before trait body.");
    std::vector<std::unique_ptr<Stmt>> methods;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        methods.push_back(function("method", false));
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after trait body.");
    return std::make_unique<TraitStmt>(name, std::vector<Token>(), std::move(methods));
}

std::unique_ptr<Stmt> PrattParser::implDeclaration() {
    std::optional<Token> traitName = consume(TokenType::IDENTIFIER, "Expect trait name.");
    consume(TokenType::FOR, "Expect 'for' after trait name.");
    Token structName = consume(TokenType::IDENTIFIER, "Expect struct name.");

    consume(TokenType::LEFT_BRACE, "Expect '{' before impl body.");
    std::vector<std::unique_ptr<Stmt>> methods;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        consume(TokenType::FUNC, "Expect 'func' before method declaration.");
        methods.push_back(function("method"));
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after impl body.");
    return std::make_unique<ImplStmt>(structName, traitName, std::vector<TypeInfo>(), std::move(methods));
}

std::unique_ptr<Stmt> PrattParser::importStatement() {
    if (match(TokenType::STRING)) {
        Token path = previous();
        consume(TokenType::SEMICOLON, "Expect ';' after import path.");
        return std::make_unique<ImportStmt>(path);
    } else {
        Token path = consume(TokenType::IDENTIFIER, "Expect module path.");
        consume(TokenType::SEMICOLON, "Expect ';' after import path.");
        return std::make_unique<ImportStmt>(path, true);
    }
}

std::unique_ptr<Stmt> PrattParser::enumDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect enum name.");
    consume(TokenType::LEFT_BRACE, "Expect '{' before enum body.");
    std::vector<Token> members;
    if (!check(TokenType::RIGHT_BRACE)) {
        do {
            members.push_back(consume(TokenType::IDENTIFIER, "Expect enum member name."));
        } while (match(TokenType::COMMA));
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after enum body.");
    return std::make_unique<EnumStmt>(name, std::move(members));
}
