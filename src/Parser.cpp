#include "Parser.h"
#include "AST/AST.h"
#include <iostream>
#include <vector>
#include <memory>
#include <functional>
#include <map>

namespace Chtholly {

// Define the function signatures for our Pratt parser.
// A prefix function doesn't have a left operand.
using PrefixParseFn = std::function<std::unique_ptr<Expr>(Parser*)>;
// An infix function has a left operand.
using InfixParseFn = std::function<std::unique_ptr<Expr>(Parser*, std::unique_ptr<Expr>)>;

// A rule for each token type, containing its parsing functions and precedence.
struct ParseRule {
    PrefixParseFn prefix;
    InfixParseFn infix;
    Precedence precedence;
};

// --- Function Prototypes for Parsing Rules ---
std::unique_ptr<Expr> grouping(Parser* parser);
std::unique_ptr<Expr> unary(Parser* parser);
std::unique_ptr<Expr> binary(Parser* parser, std::unique_ptr<Expr> left);
std::unique_ptr<Expr> number(Parser* parser);
std::unique_ptr<Expr> boolean(Parser* parser);
std::unique_ptr<Expr> variable(Parser* parser);

// --- Rules Table ---
static std::map<TokenType, ParseRule> rules;

// Initialize the parsing rules table.
void initializeRules() {
    rules[TokenType::LParen]        = {grouping, nullptr,    PREC_NONE};
    rules[TokenType::Minus]         = {unary,    binary,     PREC_TERM};
    rules[TokenType::Plus]          = {nullptr,  binary,     PREC_TERM};
    rules[TokenType::Slash]         = {nullptr,  binary,     PREC_FACTOR};
    rules[TokenType::Star]          = {nullptr,  binary,     PREC_FACTOR};
    rules[TokenType::Equal]         = {nullptr,  binary,     PREC_EQUALITY};
    rules[TokenType::NotEqual]      = {nullptr,  binary,     PREC_EQUALITY};
    rules[TokenType::Less]          = {nullptr,  binary,     PREC_COMPARISON};
    rules[TokenType::LessEqual]     = {nullptr,  binary,     PREC_COMPARISON};
    rules[TokenType::Greater]       = {nullptr,  binary,     PREC_COMPARISON};
    rules[TokenType::GreaterEqual]  = {nullptr,  binary,     PREC_COMPARISON};
    rules[TokenType::Integer]       = {number,   nullptr,    PREC_NONE};
    rules[TokenType::Boolean]       = {boolean,  nullptr,    PREC_NONE};
    rules[TokenType::Identifier]    = {variable, nullptr,    PREC_NONE};
}

static ParseRule* getRule(TokenType type) {
    if (rules.find(type) != rules.end()) {
        return &rules[type];
    }
    return nullptr;
}


// --- Parser Implementation ---

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {
    initializeRules();
}

std::vector<std::unique_ptr<Stmt>> Parser::parse() {
    std::vector<std::unique_ptr<Stmt>> statements;
    while (!isAtEnd()) {
        statements.push_back(declaration());
    }
    return statements;
}

std::unique_ptr<Stmt> Parser::declaration() {
    try {
        if (match({TokenType::Func})) return functionDeclaration();
        if (match({TokenType::Let})) return varStatement();
        if (match({TokenType::Struct})) return structDeclaration();
        return statement();
    } catch (const std::runtime_error& error) {
        synchronize();
        hadError = true;
        std::cerr << error.what() << std::endl;
        return nullptr;
    }
}

std::unique_ptr<Stmt> Parser::varStatement() {
    auto var = varDeclaration();
    consume(TokenType::Semicolon, "Expect ';' after variable declaration.");
    return var;
}

std::unique_ptr<Stmt> Parser::varDeclaration() {
    Token name = consume(TokenType::Identifier, "Expect variable name.");
    Token type;
    type.type = TokenType::Unknown;
    if (match({TokenType::Colon})) {
        type = consume(TokenType::Identifier, "Expect type annotation.");
    }
    std::unique_ptr<Expr> initializer = nullptr;
    if (match({TokenType::Assign})) {
        initializer = expression();
    }
    return std::make_unique<VarDeclStmt>(name, type, std::move(initializer));
}

std::unique_ptr<Stmt> Parser::statement() {
    if (match({TokenType::If})) return ifStatement();
    if (match({TokenType::While})) return whileStatement();
    if (match({TokenType::For})) return forStatement();
    if (match({TokenType::Switch})) return switchStatement();
    if (match({TokenType::Break})) return breakStatement();
    if (match({TokenType::Fallthrough})) return fallthroughStatement();
    if (match({TokenType::LBrace})) return std::make_unique<BlockStmt>(block());
    auto expr = expression();
    consume(TokenType::Semicolon, "Expect ';' after expression statement.");
    return std::make_unique<ExprStmt>(std::move(expr));
}

std::unique_ptr<Stmt> Parser::ifStatement() {
    consume(TokenType::LParen, "Expect '(' after 'if'.");
    auto condition = expression();
    consume(TokenType::RParen, "Expect ')' after if condition.");
    auto thenBranch = statement();
    std::unique_ptr<Stmt> elseBranch = nullptr;
    if (match({TokenType::Else})) {
        elseBranch = statement();
    }
    return std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

std::unique_ptr<Stmt> Parser::whileStatement() {
    consume(TokenType::LParen, "Expect '(' after 'while'.");
    auto condition = expression();
    consume(TokenType::RParen, "Expect ')' after while condition.");
    auto body = statement();
    return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
}

std::unique_ptr<Stmt> Parser::forStatement() {
    consume(TokenType::LParen, "Expect '(' after 'for'.");

    std::unique_ptr<Stmt> initializer;
    if (match({TokenType::Semicolon})) {
        initializer = nullptr;
    } else if (match({TokenType::Let})) {
        initializer = varStatement();
    } else {
        initializer = std::make_unique<ExprStmt>(expression());
    }

    std::unique_ptr<Expr> condition = nullptr;
    if (!check(TokenType::Semicolon)) {
        condition = expression();
    }
    consume(TokenType::Semicolon, "Expect ';' after loop condition.");

    std::unique_ptr<Expr> increment = nullptr;
    if (!check(TokenType::RParen)) {
        increment = expression();
    }
    consume(TokenType::RParen, "Expect ')' after for clauses.");

    auto body = statement();

    if (increment != nullptr) {
        std::vector<std::unique_ptr<Stmt>> blockStmts;
        blockStmts.push_back(std::move(body));
        blockStmts.push_back(std::make_unique<ExprStmt>(std::move(increment)));
        body = std::make_unique<BlockStmt>(std::move(blockStmts));
    }

    if (condition == nullptr) {
        condition = std::make_unique<BooleanLiteralExpr>(true);
    }

    body = std::make_unique<WhileStmt>(std::move(condition), std::move(body));

    if (initializer != nullptr) {
        std::vector<std::unique_ptr<Stmt>> blockStmts;
        blockStmts.push_back(std::move(initializer));
        blockStmts.push_back(std::move(body));
        body = std::make_unique<BlockStmt>(std::move(blockStmts));
    }

    return body;
}

std::unique_ptr<Stmt> Parser::switchStatement() {
    consume(TokenType::LParen, "Expect '(' after 'switch'.");
    auto condition = expression();
    consume(TokenType::RParen, "Expect ')' after switch condition.");
    consume(TokenType::LBrace, "Expect '{' before switch cases.");

    std::vector<std::unique_ptr<Stmt>> cases;
    while (!check(TokenType::RBrace) && !isAtEnd()) {
        consume(TokenType::Case, "Expect 'case' in switch statement.");
        auto caseExpr = expression();
        consume(TokenType::Colon, "Expect ':' after case expression.");
        auto body = statement();
        cases.push_back(std::make_unique<CaseStmt>(std::move(caseExpr), std::move(body)));
    }

    consume(TokenType::RBrace, "Expect '}' after switch cases.");
    return std::make_unique<SwitchStmt>(std::move(condition), std::move(cases));
}

std::unique_ptr<Stmt> Parser::breakStatement() {
    consume(TokenType::Semicolon, "Expect ';' after 'break'.");
    return std::make_unique<BreakStmt>();
}

std::unique_ptr<Stmt> Parser::fallthroughStatement() {
    consume(TokenType::Semicolon, "Expect ';' after 'fallthrough'.");
    return std::make_unique<FallthroughStmt>();
}

std::unique_ptr<Stmt> Parser::functionDeclaration() {
    Token name = consume(TokenType::Identifier, "Expect function name.");
    consume(TokenType::LParen, "Expect '(' after function name.");

    std::vector<FunctionStmt::Parameter> params;
    if (!check(TokenType::RParen)) {
        do {
            Token paramName = consume(TokenType::Identifier, "Expect parameter name.");
            consume(TokenType::Colon, "Expect ':' after parameter name.");
            Token paramType = consume(TokenType::Identifier, "Expect parameter type.");
            params.push_back({paramName, paramType});
        } while (match({TokenType::Comma}));
    }

    consume(TokenType::RParen, "Expect ')' after parameters.");

    Token returnType;
    returnType.type = TokenType::Unknown;
    if (match({TokenType::Arrow})) {
        returnType = consume(TokenType::Identifier, "Expect return type.");
    }

    consume(TokenType::LBrace, "Expect '{' before function body.");
    auto body = std::make_unique<BlockStmt>(block());
    return std::make_unique<FunctionStmt>(name, std::move(params), returnType, std::move(body));
}

std::unique_ptr<Stmt> Parser::structDeclaration() {
    Token name = consume(TokenType::Identifier, "Expect struct name.");
    consume(TokenType::LBrace, "Expect '{' before struct body.");

    std::vector<std::unique_ptr<VarDeclStmt>> fields;
    std::vector<std::unique_ptr<FunctionStmt>> methods;

    while (!check(TokenType::RBrace) && !isAtEnd()) {
        if (peekNext().type == TokenType::LParen) {
            auto method = functionDeclaration();
            if (auto funcStmt = dynamic_cast<FunctionStmt*>(method.get())) {
                method.release();
                methods.push_back(std::unique_ptr<FunctionStmt>(funcStmt));
            }
        } else {
            auto field = varDeclaration();
            if (auto varDeclStmt = dynamic_cast<VarDeclStmt*>(field.get())) {
                field.release();
                fields.push_back(std::unique_ptr<VarDeclStmt>(varDeclStmt));
            }
            if (!check(TokenType::RBrace)) {
                consume(TokenType::Comma, "Expect ',' after field declaration.");
            }
        }
    }

    consume(TokenType::RBrace, "Expect '}' after struct body.");
    return std::make_unique<StructStmt>(name, std::move(fields), std::move(methods));
}


std::vector<std::unique_ptr<Stmt>> Parser::block() {
    std::vector<std::unique_ptr<Stmt>> statements;
    while (!check(TokenType::RBrace) && !isAtEnd()) {
        statements.push_back(declaration());
    }
    consume(TokenType::RBrace, "Expect '}' after block.");
    return statements;
}

std::unique_ptr<Expr> Parser::expression() {
    return assignment();
}

std::unique_ptr<Expr> Parser::assignment() {
    auto expr = parsePrecedence(PREC_OR);

    if (match({TokenType::Assign})) {
        auto value = assignment();

        if (auto* varExpr = dynamic_cast<VariableExpr*>(expr.get())) {
            return std::make_unique<AssignExpr>(varExpr->name, std::move(value));
        }

        throw std::runtime_error("Invalid assignment target.");
    }

    return expr;
}

std::unique_ptr<Expr> Parser::parsePrecedence(Precedence precedence) {
    advance();
    PrefixParseFn prefixRule = getRule(previous().type)->prefix;
    if (prefixRule == nullptr) {
        throw std::runtime_error("Expect expression");
    }

    auto leftExpr = prefixRule(this);

    while (getRule(peek().type) != nullptr && precedence <= getRule(peek().type)->precedence) {
        advance();
        InfixParseFn infixRule = getRule(previous().type)->infix;
        leftExpr = infixRule(this, std::move(leftExpr));
    }

    return leftExpr;
}

// --- Helper Methods ---

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw std::runtime_error(message);
}

bool Parser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

void Parser::synchronize() {
    advance();
    while (!isAtEnd()) {
        if (previous().type == TokenType::Semicolon) return;
        switch (peek().type) {
            case TokenType::Func:
            case TokenType::Let:
            case TokenType::For:
            case TokenType::If:
            case TokenType::While:
            case TokenType::Return:
                return;
            default:
                break;
        }
        advance();
    }
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::check(TokenType type) const { return !isAtEnd() && peek().type == type; }
bool Parser::isAtEnd() const { return peek().type == TokenType::EndOfFile; }
Token Parser::peek() const { return tokens.at(current); }
Token Parser::peekNext() const {
    if (isAtEnd()) return peek();
    return tokens.at(current + 1);
}
Token Parser::previous() const { return tokens.at(current - 1); }


// --- Parsing Rule Implementations ---

std::unique_ptr<Expr> grouping(Parser* parser) {
    auto expr = parser->expression();
    parser->consume(TokenType::RParen, "Expect ')' after expression.");
    return std::make_unique<GroupingExpr>(std::move(expr));
}

std::unique_ptr<Expr> unary(Parser* parser) {
    Token op = parser->previous();
    auto right = parser->parsePrecedence(PREC_UNARY);
    return std::make_unique<UnaryExpr>(op, std::move(right));
}

std::unique_ptr<Expr> binary(Parser* parser, std::unique_ptr<Expr> left) {
    Token op = parser->previous();
    ParseRule* rule = getRule(op.type);
    auto right = parser->parsePrecedence((Precedence)(rule->precedence + 1));
    return std::make_unique<BinaryExpr>(std::move(left), op, std::move(right));
}

std::unique_ptr<Expr> number(Parser* parser) {
    return std::make_unique<NumberLiteralExpr>(parser->previous());
}

std::unique_ptr<Expr> boolean(Parser* parser) {
    return std::make_unique<BooleanLiteralExpr>(parser->previous());
}

std::unique_ptr<Expr> variable(Parser* parser) {
    return std::make_unique<VariableExpr>(parser->previous());
}

} // namespace Chtholly
