#include "Parser.h"
#include "Error.h"

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

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
        if (match({TokenType::FUNC})) return function("function", true);
        if (match({TokenType::LET, TokenType::MUT})) return letDeclaration();
        return statement();
    } catch (ParseError& error) {
        synchronize();
        return nullptr;
    }
}

std::unique_ptr<Stmt> Parser::structDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect struct name.");

    std::vector<Token> traitNames;
    if (match({TokenType::IMPL})) {
        do {
            traitNames.push_back(consume(TokenType::IDENTIFIER, "Expect trait name."));
        } while (match({TokenType::COMMA}));
    }

    consume(TokenType::LEFT_BRACE, "Expect '{' before struct body.");
    std::vector<std::unique_ptr<LetStmt>> fields;
    std::vector<std::unique_ptr<FunctionStmt>> methods;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (match({TokenType::LET, TokenType::MUT})) {
            fields.push_back(std::unique_ptr<LetStmt>(static_cast<LetStmt*>(letDeclaration().release())));
        } else if (match({TokenType::FUNC})) {
            methods.push_back(std::unique_ptr<FunctionStmt>(static_cast<FunctionStmt*>(function("method", true).release())));
        } else {
            ErrorReporter::error(peek().line, "Expect 'let', 'mut', or 'func' in struct body.");
            synchronize();
        }
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after struct body.");
    return std::make_unique<StructStmt>(name, std::move(traitNames), std::move(fields), std::move(methods));
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
    std::vector<Token> generics;
    if (match({TokenType::LESS})) {
        do {
            generics.push_back(consume(TokenType::IDENTIFIER, "Expect generic type name."));
        } while (match({TokenType::COMMA}));
        consume(TokenType::GREATER, "Expect '>' after generic type list.");
    }
    consume(TokenType::LEFT_PAREN, "Expect '(' after " + kind + " name.");
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
    consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");

    std::optional<TypeInfo> returnType;
    if (match({TokenType::MINUS})) {
        consume(TokenType::GREATER, "Expect '>' after '-' for return type arrow.");
        returnType = parseType();
    }

    std::vector<std::unique_ptr<Stmt>> body;
    if (body_required) {
        consume(TokenType::LEFT_BRACE, "Expect '{' before " + kind + " body.");
        body = block();
    } else {
        consume(TokenType::SEMICOLON, "Expect ';' after " + kind + " declaration.");
    }
    return std::make_unique<FunctionStmt>(name, std::move(generics), std::move(parameters), std::move(body), std::move(returnType));
}

std::unique_ptr<Stmt> Parser::letDeclaration() {
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

std::unique_ptr<Expr> Parser::expression() {
    return assignment();
}

std::unique_ptr<Expr> Parser::assignment() {
    auto expr = logic_or();

    if (match({TokenType::EQUAL})) {
        Token equals = previous();
        auto value = assignment();

        if (auto* var = dynamic_cast<VariableExpr*>(expr.get())) {
            return std::make_unique<AssignExpr>(var->name, std::move(value));
        } else if (auto* get = dynamic_cast<GetExpr*>(expr.get())) {
            return std::make_unique<SetExpr>(std::move(get->object), get->name, std::move(value));
        }

        ErrorReporter::error(equals.line, "Invalid assignment target.");
    }

    return expr;
}

std::unique_ptr<Expr> Parser::logic_or() {
    auto expr = logic_and();

    while (match({TokenType::PIPE_PIPE})) {
        Token op = previous();
        auto right = logic_and();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::logic_and() {
    auto expr = equality();

    while (match({TokenType::AMPERSAND_AMPERSAND})) {
        Token op = previous();
        auto right = equality();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::equality() {
    auto expr = comparison();

    while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
        Token op = previous();
        auto right = comparison();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::comparison() {
    auto expr = term();

    while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL})) {
        Token op = previous();
        auto right = term();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::term() {
    auto expr = factor();

    while (match({TokenType::MINUS, TokenType::PLUS})) {
        Token op = previous();
        auto right = factor();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::factor() {
    auto expr = unary();

    while (match({TokenType::SLASH, TokenType::STAR})) {
        Token op = previous();
        auto right = unary();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::unary() {
    if (match({TokenType::BANG, TokenType::MINUS})) {
        Token op = previous();
        auto right = unary();
        return std::make_unique<UnaryExpr>(op, std::move(right));
    }

    if (match({TokenType::AMPERSAND})) {
        bool isMutable = match({TokenType::MUT});
        auto expr = unary();
        return std::make_unique<BorrowExpr>(std::move(expr), isMutable);
    }

    return call();
}

std::unique_ptr<Expr> Parser::call() {
    auto expr = primary();

    while (true) {
        if (match({TokenType::LEFT_PAREN})) {
            expr = finishCall(std::move(expr), {});
        } else if (match({TokenType::DOT})) {
            Token name = consume(TokenType::IDENTIFIER, "Expect property name after '.'.");
            expr = std::make_unique<GetExpr>(std::move(expr), name);
        } else if (peek().type == TokenType::LESS && LA_is_generic_call()) {
            consume(TokenType::LESS, "Expect '<' for generic arguments.");
            std::vector<TypeInfo> generic_args;
            do {
                generic_args.push_back(parseType());
            } while (match({TokenType::COMMA}));
            consume(TokenType::GREATER, "Expect '>' after generic arguments.");
            consume(TokenType::LEFT_PAREN, "Expect '(' after generic arguments.");
            expr = finishCall(std::move(expr), std::move(generic_args));
        } else {
            break;
        }
    }

    return expr;
}

std::unique_ptr<Expr> Parser::finishCall(std::unique_ptr<Expr> callee, std::vector<TypeInfo> generic_args) {
    std::vector<std::unique_ptr<Expr>> arguments;
    if (peek().type != TokenType::RIGHT_PAREN) {
        do {
            if (arguments.size() >= 255) {
                ErrorReporter::error(peek().line, "Can't have more than 255 arguments.");
            }
            arguments.push_back(expression());
        } while (match({TokenType::COMMA}));
    }

    Token paren = consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");

    return std::make_unique<CallExpr>(std::move(callee), paren, std::move(arguments), std::move(generic_args));
}

std::unique_ptr<Expr> Parser::primary() {
    if (match({TokenType::FALSE})) return std::make_unique<LiteralExpr>(false);
    if (match({TokenType::TRUE})) return std::make_unique<LiteralExpr>(true);

    if (match({TokenType::NUMBER, TokenType::STRING})) {
        return std::make_unique<LiteralExpr>(previous().literal);
    }

    if (match({TokenType::IDENTIFIER})) {
        return std::make_unique<VariableExpr>(previous());
    }

    if (match({TokenType::LEFT_BRACKET})) {
        consume(TokenType::RIGHT_BRACKET, "Expect ']' after lambda capture list.");
        consume(TokenType::LEFT_PAREN, "Expect '(' after lambda capture list.");
        std::vector<std::pair<Token, TypeInfo>> parameters;
        if (peek().type != TokenType::RIGHT_PAREN) {
            do {
                Token paramName = consume(TokenType::IDENTIFIER, "Expect parameter name.");
                consume(TokenType::COLON, "Expect ':' after parameter name.");
                parameters.emplace_back(paramName, parseType());
            } while (match({TokenType::COMMA}));
        }
        consume(TokenType::RIGHT_PAREN, "Expect ')' after lambda parameters.");

        std::optional<TypeInfo> returnType;
        if (match({TokenType::MINUS})) {
            consume(TokenType::GREATER, "Expect '>' after '-' for return type arrow.");
            returnType = parseType();
        }

        consume(TokenType::LEFT_BRACE, "Expect '{' before lambda body.");
        auto body = block();
        return std::make_unique<LambdaExpr>(std::move(parameters), std::move(body), std::move(returnType));
    }

    if (match({TokenType::LEFT_PAREN})) {
        auto expr = expression();
        if (peek().type != TokenType::RIGHT_PAREN) {
            ErrorReporter::error(peek().line, "Expect ')' after expression.");
        }
        advance();
        return std::make_unique<GroupingExpr>(std::move(expr));
    }

    ErrorReporter::error(peek().line, "Expect expression.");
    throw std::runtime_error("Expect expression.");
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
            type.returnType = std::make_unique<TypeInfo>(parseType());
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
