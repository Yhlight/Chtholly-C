#include "Parser.h"
#include "PrattParser.h"
#include "Error.h"

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {
    prattParser = std::make_unique<PrattParser>(*this);
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
        if (match({TokenType::IMPORT})) return importDeclaration();
        if (match({TokenType::STRUCT})) return structDeclaration();
        if (match({TokenType::TRAIT})) return traitDeclaration();
        if (match({TokenType::IMPL})) return implDeclaration();
        if (match({TokenType::FUNC})) return function("function", true);
        if (match({TokenType::LET, TokenType::MUT})) return variableDeclaration();
        return statement();
    } catch (ParseError& error) {
        synchronize();
        return nullptr;
    }
}

std::unique_ptr<Stmt> Parser::structDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect struct name.");
    consume(TokenType::LEFT_BRACE, "Expect '{' before struct body.");
    std::vector<std::unique_ptr<LetStmt>> fields;
    while (peek().type != TokenType::RIGHT_BRACE && !isAtEnd()) {
        if (match({TokenType::LET, TokenType::MUT})) {
            fields.push_back(std::unique_ptr<LetStmt>(static_cast<LetStmt*>(variableDeclaration().release())));
        } else {
            ErrorReporter::error(peek().line, "Expect 'let' or 'mut' in struct body.");
            synchronize(); // Skip the invalid token and attempt to continue parsing
        }
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after struct body.");
    return std::make_unique<StructStmt>(name, std::move(fields));
}

std::unique_ptr<Stmt> Parser::traitDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect trait name.");
    consume(TokenType::LEFT_BRACE, "Expect '{' before trait body.");
    std::vector<std::unique_ptr<Stmt>> methods;
    while (peek().type != TokenType::RIGHT_BRACE && !isAtEnd()) {
        methods.push_back(function("method", false));
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after trait body.");
    return std::make_unique<TraitStmt>(name, std::move(methods));
}

std::unique_ptr<Stmt> Parser::implDeclaration() {
    Token traitName = consume(TokenType::IDENTIFIER, "Expect trait name.");
    consume(TokenType::FOR, "Expect 'for' after trait name.");
    Token structName = consume(TokenType::IDENTIFIER, "Expect struct name.");
    consume(TokenType::LEFT_BRACE, "Expect '{' before impl body.");
    std::vector<std::unique_ptr<Stmt>> methods;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        methods.push_back(function("method", true));
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after impl body.");
    return std::make_unique<ImplStmt>(structName, traitName, std::move(methods));
}

std::unique_ptr<Stmt> Parser::importDeclaration() {
    if (match({TokenType::STRING})) {
        Token path = previous();
        consume(TokenType::SEMICOLON, "Expect ';' after module path.");
        return std::make_unique<ImportStmt>(path, false);
    } else {
        Token name = consume(TokenType::IDENTIFIER, "Expect module name.");
        consume(TokenType::SEMICOLON, "Expect ';' after module name.");
        return std::make_unique<ImportStmt>(name, true);
    }
}

std::unique_ptr<Stmt> Parser::function(const std::string& kind, bool body_required) {
    Token name = consume(TokenType::IDENTIFIER, "Expect " + kind + " name.");
    auto generics = parseGenerics();
    consume(TokenType::LEFT_PAREN, "Expect '(' after " + kind + " name.");
    auto parameters = parseParameters();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");
    auto returnType = parseReturnType();

    std::vector<std::unique_ptr<Stmt>> body;
    if (body_required) {
        consume(TokenType::LEFT_BRACE, "Expect '{' before " + kind + " body.");
        body = block();
    } else {
        consume(TokenType::SEMICOLON, "Expect ';' after " + kind + " declaration.");
    }
    return std::make_unique<FunctionStmt>(name, std::move(generics), std::move(parameters), std::move(body), std::move(returnType));
}

std::vector<std::pair<Token, TypeInfo>> Parser::parseParameters() {
    std::vector<std::pair<Token, TypeInfo>> parameters;
    if (peek().type != TokenType::RIGHT_PAREN) {
        do {
            if (parameters.size() >= 255) {
                ErrorReporter::error(peek().line, "Can't have more than 255 parameters.");
            }
            Token paramName = consume(TokenType::IDENTIFIER, "Expect parameter name.");
            consume(TokenType::COLON, "Expect ':' after parameter name.");
            parameters.emplace_back(paramName, parseType());
        } while (match({TokenType::COMMA}));
    }
    return parameters;
}

std::vector<Token> Parser::parseGenerics() {
    std::vector<Token> generics;
    if (match({TokenType::LESS})) {
        do {
            generics.push_back(consume(TokenType::IDENTIFIER, "Expect generic type name."));
        } while (match({TokenType::COMMA}));
        consume(TokenType::GREATER, "Expect '>' after generic type list.");
    }
    return generics;
}

std::optional<TypeInfo> Parser::parseReturnType() {
    if (match({TokenType::MINUS})) {
        consume(TokenType::GREATER, "Expect '>' after '-' for return type arrow.");
        return parseType();
    }
    return std::nullopt;
}

std::unique_ptr<Stmt> Parser::variableDeclaration() {
    bool isMutable = previous().type == TokenType::MUT;
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");
    std::optional<TypeInfo> type;
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

std::unique_ptr<Stmt> Parser::statement() {
    if (match({TokenType::IF})) return ifStatement();
    if (match({TokenType::WHILE})) return whileStatement();
    if (match({TokenType::LEFT_BRACE})) return std::make_unique<BlockStmt>(block());
    if (match({TokenType::RETURN})) return returnStatement();
    return expressionStatement();
}

std::unique_ptr<Stmt> Parser::returnStatement() {
    Token keyword = previous();
    std::unique_ptr<Expr> value = nullptr;
    if (peek().type != TokenType::SEMICOLON) {
        value = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after return value.");
    return std::make_unique<ReturnStmt>(keyword, std::move(value));
}

std::unique_ptr<Stmt> Parser::ifStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
    auto condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");

    auto thenBranch = statement();
    std::unique_ptr<Stmt> elseBranch = nullptr;
    if (match({TokenType::ELSE})) {
        elseBranch = statement();
    }

    return std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

std::unique_ptr<Stmt> Parser::whileStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
    auto condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after while condition.");
    auto body = statement();

    return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
}

std::vector<std::unique_ptr<Stmt>> Parser::block() {
    std::vector<std::unique_ptr<Stmt>> statements;

    while (!isAtEnd() && peek().type != TokenType::RIGHT_BRACE) {
        statements.push_back(declaration());
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
    return statements;
}

std::unique_ptr<Stmt> Parser::expressionStatement() {
    auto expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    return std::make_unique<ExpressionStmt>(std::move(expr));
}

Parser::~Parser() = default;

std::unique_ptr<Expr> Parser::expression() {
    return prattParser->parse();
}

TypeInfo Parser::parseType() {
    TypeInfo type;
    if (match({TokenType::AMPERSAND})) {
        type.isReference = true;
        if (match({TokenType::MUT})) {
            type.isMutable = true;
        }
    }
    if (match({TokenType::FUNCTION})) {
        type.baseType = previous();
        consume(TokenType::LEFT_PAREN, "Expect '(' after 'function'.");
        if (!match({TokenType::RIGHT_PAREN})) {
            do {
                type.params.push_back(parseType());
            } while (match({TokenType::COMMA}));
            consume(TokenType::RIGHT_PAREN, "Expect ')' after function type parameters.");
        }
        if (match({TokenType::MINUS})) {
            consume(TokenType::GREATER, "Expect '>' after '-' for function type return arrow.");
            type.returnType = std::make_shared<TypeInfo>(parseType());
        }
    } else {
        type.baseType = consume(TokenType::IDENTIFIER, "Expect type name.");
    }
    return type;
}

bool Parser::LA_is_generic_call() {
    int saved = current;
    try {
        if (!match({TokenType::LESS})) {
            current = saved;
            return false;
        }
        do {
            parseType();
        } while (match({TokenType::COMMA}));
        if (match({TokenType::GREATER})) {
            current = saved;
            return true;
        }
    } catch (ParseError& error) {
        // fall through
    }
    current = saved;
    return false;
}

bool Parser::match(const std::vector<TokenType>& types) {
    for (auto type : types) {
        if (!isAtEnd() && peek().type == type) {
            advance();
            return true;
        }
    }
    return false;
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::isAtEnd() {
    return peek().type == TokenType::END_OF_FILE;
}

Token Parser::peek() {
    return tokens[current];
}

Token Parser::previous() {
    return tokens[current - 1];
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (!isAtEnd() && peek().type == type) return advance();
    throw error(peek(), message);
}

Parser::ParseError Parser::error(const Token& token, const std::string& message) {
    ErrorReporter::error(token.line, message);
    return ParseError(message);
}

void Parser::synchronize() {
    advance();

    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return;

        switch (peek().type) {
            case TokenType::FUNC:
            case TokenType::LET:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::RETURN:
                return;
        }

        advance();
    }
}
