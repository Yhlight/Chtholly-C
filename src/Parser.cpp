#include "Parser.h"
#include "PrattParser.h"
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
        if (match({TokenType::IMPL})) return implDeclaration();
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
    consume(TokenType::LEFT_BRACE, "Expect '{' before struct body.");
    std::vector<std::unique_ptr<LetStmt>> fields;
    while (!match({TokenType::RIGHT_BRACE}) && !isAtEnd()) {
        bool is_public = true;
        if (match({TokenType::PUBLIC})) {
            // public is default, so we just consume the token
        } else if (match({TokenType::PRIVATE})) {
            is_public = false;
        }

        if (match({TokenType::LET, TokenType::MUT})) {
            auto let_stmt = std::unique_ptr<LetStmt>(static_cast<LetStmt*>(letDeclaration().release()));
            let_stmt->is_public = is_public;
            fields.push_back(std::move(let_stmt));
        } else {
            ErrorReporter::error(peek().line, "Expect 'let' or 'mut' in struct body.");
            synchronize();
        }
    }
    return std::make_unique<StructStmt>(name, std::move(fields));
}

std::unique_ptr<Stmt> Parser::traitDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect trait name.");
    std::vector<Token> generics;
    if (match({TokenType::LESS})) {
        do {
            generics.push_back(consume(TokenType::IDENTIFIER, "Expect generic type name."));
        } while (match({TokenType::COMMA}));
        consume(TokenType::GREATER, "Expect '>' after generic type list.");
    }
    consume(TokenType::LEFT_BRACE, "Expect '{' before trait body.");
    std::vector<std::unique_ptr<Stmt>> methods;
    while (peek().type != TokenType::RIGHT_BRACE && !isAtEnd()) {
        methods.push_back(function("method", false));
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after trait body.");
    return std::make_unique<TraitStmt>(name, std::move(generics), std::move(methods));
}

std::unique_ptr<Stmt> Parser::implDeclaration() {
    Token name1 = consume(TokenType::IDENTIFIER, "Expect trait name.");
    std::optional<Token> traitName = name1;
    std::vector<TypeInfo> generics;
    if (match({TokenType::LESS})) {
        do {
            generics.push_back(parseType());
        } while (match({TokenType::COMMA}));
        consume(TokenType::GREATER, "Expect '>' after generic type list.");
    }

    consume(TokenType::FOR, "Expect 'for' after trait name.");
    Token structName = consume(TokenType::IDENTIFIER, "Expect struct name.");
    consume(TokenType::LEFT_BRACE, "Expect '{' before impl body.");
    std::vector<std::unique_ptr<Stmt>> methods;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        methods.push_back(function("method", true));
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after impl body.");
    return std::make_unique<ImplStmt>(structName, traitName, std::move(generics), std::move(methods));
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
    Token name;
    if (match({TokenType::OPERATOR})) {
        consume(TokenType::COLON_COLON, "Expect '::' after 'operator'.");
        Token op_name = consume(TokenType::IDENTIFIER, "Expect operator name after '::'.");
        name = Token{TokenType::IDENTIFIER, "operator::" + op_name.lexeme, std::monostate{}, op_name.line};
    } else {
        name = consume(TokenType::IDENTIFIER, "Expect " + kind + " name.");
    }
    std::vector<Token> generics;
    if (match({TokenType::LESS})) {
        do {
            generics.push_back(consume(TokenType::IDENTIFIER, "Expect generic type name."));
        } while (match({TokenType::COMMA}));
        consume(TokenType::GREATER, "Expect '>' after generic type list.");
    }
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
    return std::make_unique<LetStmt>(name, std::move(type), std::move(initializer), isMutable, true);
}

std::unique_ptr<Stmt> Parser::statement() {
    if (match({TokenType::IF})) return ifStatement();
    if (match({TokenType::WHILE})) return whileStatement();
    if (match({TokenType::LEFT_BRACE})) return std::make_unique<BlockStmt>(block());
    if (match({TokenType::RETURN})) return returnStatement();
    if (match({TokenType::SWITCH})) return switchStatement();
    if (match({TokenType::BREAK})) {
        Token keyword = previous();
        consume(TokenType::SEMICOLON, "Expect ';' after 'break'.");
        return std::make_unique<BreakStmt>(keyword);
    }
    if (match({TokenType::FALLTHROUGH})) {
        Token keyword = previous();
        consume(TokenType::SEMICOLON, "Expect ';' after 'fallthrough'.");
        return std::make_unique<FallthroughStmt>(keyword);
    }
    return expressionStatement();
}

std::unique_ptr<Stmt> Parser::switchStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'switch'.");
    auto expression = this->expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after switch expression.");
    consume(TokenType::LEFT_BRACE, "Expect '{' after switch expression.");

    std::vector<CaseStmt> cases;
    std::optional<CaseStmt> defaultCase;

    while (!match({TokenType::RIGHT_BRACE}) && !isAtEnd()) {
        if (match({TokenType::CASE})) {
            auto condition = this->expression();
            consume(TokenType::COLON, "Expect ':' after case value.");
            auto body = statement();
            cases.push_back({std::move(condition), std::move(body)});
        } else if (match({TokenType::DEFAULT})) {
            consume(TokenType::COLON, "Expect ':' after 'default'.");
            defaultCase = CaseStmt{nullptr, statement()};
        } else {
            ErrorReporter::error(peek().line, "Expect 'case' or 'default' in switch statement.");
            synchronize();
        }
    }

    return std::make_unique<SwitchStmt>(std::move(expression), std::move(cases), std::move(defaultCase));
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
    PrattParser prattParser(*this);
    return prattParser.parse();
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

std::optional<TypeInfo> Parser::parseReturnType() {
    if (match({TokenType::MINUS})) {
        consume(TokenType::GREATER, "Expect '>' after '-' for return type arrow.");
        return parseType();
    }
    return std::nullopt;
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
            case TokenType::IMPL:
                return;
        }

        advance();
    }
}
