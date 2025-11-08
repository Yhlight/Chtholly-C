#include "Parser.h"
#include <utility>
#include <iostream>

namespace chtholly {

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

std::vector<std::unique_ptr<Stmt>> Parser::parse() {
    std::vector<std::unique_ptr<Stmt>> statements;
    while (!isAtEnd()) {
        statements.push_back(declaration());
    }
    return statements;
}

std::unique_ptr<Stmt> Parser::declaration() {
    try {
        if (match(TokenType::FUNC)) return functionDeclaration("function", true, Access::PUBLIC, nullptr);
        if (match(TokenType::STRUCT)) return structDeclaration();
        if (match(TokenType::ENUM)) return enumDeclaration();
        if (match(TokenType::TRAIT)) return traitDeclaration();
        if (match(TokenType::LET, TokenType::MUT)) return varDeclaration(true);
        return statement();
    } catch (ParseError& error) {
        synchronize();
        return nullptr;
    }
}

std::unique_ptr<VarStmt> Parser::varDeclaration(bool consume_semicolon, Access access) {
    Token keyword = previous();
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");
    std::unique_ptr<TypeExpr> type_expr = nullptr;
    if (match(TokenType::COLON)) {
        type_expr = type();
    }
    std::unique_ptr<Expr> initializer = nullptr;
    if (match(TokenType::EQUAL)) {
        initializer = expression();
    }
    if (consume_semicolon) {
        consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    }
    return std::make_unique<VarStmt>(std::move(name), std::move(type_expr), std::move(initializer), keyword.type == TokenType::MUT, access);
}

std::unique_ptr<Stmt> Parser::statement() {
    if (match(TokenType::IF)) return ifStatement();
    if (match(TokenType::WHILE)) return whileStatement();
    if (match(TokenType::FOR)) return forStatement();
    if (match(TokenType::SWITCH)) return switchStatement();
    if (match(TokenType::RETURN)) return returnStatement();
    if (match(TokenType::BREAK)) return breakStatement();
    if (match(TokenType::FALLTHROUGH)) return fallthroughStatement();
    if (match(TokenType::IMPORT)) return importStatement();
    if (match(TokenType::LEFT_BRACE)) return std::make_unique<BlockStmt>(block());
    return expressionStatement();
}

std::unique_ptr<Stmt> Parser::expressionStatement() {
    auto expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    return std::make_unique<ExpressionStmt>(std::move(expr));
}

std::unique_ptr<Stmt> Parser::ifStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
    auto condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");
    auto thenBranch = statement();
    std::unique_ptr<Stmt> elseBranch = nullptr;
    if (match(TokenType::ELSE)) {
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

std::unique_ptr<Stmt> Parser::forStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'.");

    std::unique_ptr<Stmt> initializer;
    if (match(TokenType::SEMICOLON)) {
        initializer = nullptr;
    } else if (match(TokenType::LET, TokenType::MUT)) {
        initializer = varDeclaration(false, Access::PUBLIC);
    } else {
        initializer = expressionStatement();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after loop initializer.");

    std::unique_ptr<Expr> condition = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        condition = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");

    std::unique_ptr<Expr> increment = nullptr;
    if (!check(TokenType::RIGHT_PAREN)) {
        increment = expression();
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");

    auto body = statement();

    return std::make_unique<ForStmt>(std::move(initializer), std::move(condition), std::move(increment), std::move(body));
}

std::unique_ptr<Stmt> Parser::switchStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'switch'.");
    auto value = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after switch value.");
    consume(TokenType::LEFT_BRACE, "Expect '{' before switch cases.");
    std::vector<std::unique_ptr<CaseStmt>> cases;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        cases.push_back(caseStatement());
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after switch cases.");
    return std::make_unique<SwitchStmt>(std::move(value), std::move(cases));
}

std::unique_ptr<CaseStmt> Parser::caseStatement() {
    std::unique_ptr<Expr> value = nullptr;
    if (match(TokenType::CASE)) {
        value = expression();
    } else {
        consume(TokenType::DEFAULT, "Expect 'case' or 'default'.");
        // value is already nullptr for default case
    }
    consume(TokenType::COLON, "Expect ':' after case value.");
    auto body = statement();
    return std::make_unique<CaseStmt>(std::move(value), std::move(body));
}

std::unique_ptr<Stmt> Parser::returnStatement() {
    Token keyword = previous();
    std::unique_ptr<Expr> value = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        value = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after return value.");
    return std::make_unique<ReturnStmt>(std::move(keyword), std::move(value));
}

std::unique_ptr<Stmt> Parser::breakStatement() {
    Token keyword = previous();
    consume(TokenType::SEMICOLON, "Expect ';' after 'break'.");
    return std::make_unique<BreakStmt>(std::move(keyword));
}

std::unique_ptr<Stmt> Parser::fallthroughStatement() {
    Token keyword = previous();
    consume(TokenType::SEMICOLON, "Expect ';' after 'fallthrough'.");
    return std::make_unique<FallthroughStmt>(std::move(keyword));
}

std::unique_ptr<Stmt> Parser::importStatement() {
    Token keyword = previous();
    std::variant<std::string, Token> path;
    if (match(TokenType::STRING_LITERAL)) {
        path = std::get<std::string>(previous().literal);
    } else if (match(TokenType::IDENTIFIER, TokenType::IOSTREAM, TokenType::FILESYSTEM, TokenType::OPERATOR, TokenType::REFLECT, TokenType::META, TokenType::UTIL)) {
        path = previous();
    } else {
        throw error(peek(), "Expect module name or file path after 'import'.");
    }
    consume(TokenType::SEMICOLON, "Expect ';' after import statement.");
    return std::make_unique<ImportStmt>(std::move(keyword), std::move(path));
}

std::unique_ptr<FunctionStmt> Parser::functionDeclaration(const std::string& kind, bool has_body, Access access, std::unique_ptr<TypeExpr> trait_impl) {
    Token name = consume(TokenType::IDENTIFIER, "Expect " + kind + " name.");
    std::vector<Token> generic_params;
    std::map<std::string, std::vector<std::unique_ptr<TypeExpr>>> generic_constraints;
    if (match(TokenType::LESS)) {
        do {
            Token param_name = consume(TokenType::IDENTIFIER, "Expect generic type name.");
            generic_params.push_back(param_name);
            if (match(TokenType::QUESTION)) {
                std::vector<std::unique_ptr<TypeExpr>> constraints;
                do {
                    constraints.push_back(type());
                } while (match(TokenType::COMMA));
                generic_constraints[param_name.lexeme] = std::move(constraints);
            }
        } while (match(TokenType::COMMA));
        consume(TokenType::GREATER, "Expect '>' after generic type parameters.");
    }
    consume(TokenType::LEFT_PAREN, "Expect '(' after " + kind + " name.");
    std::vector<Token> parameters;
    std::vector<std::unique_ptr<TypeExpr>> param_types;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            // Handle special 'self' parameters
            if (peek().type == TokenType::AMPERSAND) {
                bool isMutable = (lookahead().type == TokenType::MUT);
                int self_offset = isMutable ? 2 : 1;

                if (current + self_offset < tokens.size() && tokens[current + self_offset].type == TokenType::SELF) {
                    advance(); // consume &
                    if (isMutable) advance(); // consume mut
                    Token self_token = consume(TokenType::SELF, "Expect 'self'.");

                    auto element_type = std::make_unique<BaseTypeExpr>(self_token);
                    param_types.push_back(std::make_unique<BorrowTypeExpr>(std::move(element_type), isMutable));
                    parameters.push_back(self_token);
                    continue;
                }
            } else if (peek().type == TokenType::SELF) {
                Token self_token = advance();
                param_types.push_back(std::make_unique<BaseTypeExpr>(self_token));
                parameters.push_back(self_token);
                continue;
            }

            // Fallback to regular parameter
            parameters.push_back(consume(TokenType::IDENTIFIER, "Expect parameter name."));
            consume(TokenType::COLON, "Expect ':' after parameter name.");
            param_types.push_back(type());
        } while (match(TokenType::COMMA));
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");
    std::unique_ptr<TypeExpr> returnType = nullptr;
    if (match(TokenType::ARROW)) {
        returnType = type();
    }

    std::unique_ptr<BlockStmt> body = nullptr;
    if (has_body) {
        consume(TokenType::LEFT_BRACE, "Expect '{' before " + kind + " body.");
        body = std::make_unique<BlockStmt>(block());
    } else {
        consume(TokenType::SEMICOLON, "Expect ';' after method signature.");
    }

    return std::make_unique<FunctionStmt>(std::move(name), std::move(generic_params), std::move(generic_constraints), std::move(parameters), std::move(param_types), std::move(returnType), std::move(body), access, std::move(trait_impl));
}

std::unique_ptr<Stmt> Parser::structDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect struct name.");

    std::vector<Token> generic_params;
    std::map<std::string, std::vector<std::unique_ptr<TypeExpr>>> generic_constraints;
    if (match(TokenType::LESS)) {
        do {
            Token param_name = consume(TokenType::IDENTIFIER, "Expect generic type name.");
            generic_params.push_back(param_name);
            if (match(TokenType::QUESTION)) {
                std::vector<std::unique_ptr<TypeExpr>> constraints;
                do {
                    constraints.push_back(type());
                } while (match(TokenType::COMMA));
                generic_constraints[param_name.lexeme] = std::move(constraints);
            }
        } while (match(TokenType::COMMA));
        consume(TokenType::GREATER, "Expect '>' after generic type parameters.");
    }

    std::vector<std::unique_ptr<Expr>> traits;
    if (match(TokenType::IMPL)) {
        do {
            traits.push_back(expression());
        } while (match(TokenType::COMMA));
    }

    consume(TokenType::LEFT_BRACE, "Expect '{' before struct body.");
    std::vector<std::unique_ptr<VarStmt>> fields;
    std::vector<std::unique_ptr<FunctionStmt>> methods;
    Access current_access = Access::PUBLIC; // Default to public

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (match(TokenType::PUBLIC)) {
            consume(TokenType::COLON, "Expect ':' after 'public'.");
            current_access = Access::PUBLIC;
        } else if (match(TokenType::PRIVATE)) {
            consume(TokenType::COLON, "Expect ':' after 'private'.");
            current_access = Access::PRIVATE;
        }

        std::unique_ptr<TypeExpr> trait_impl = nullptr;
        if (match(TokenType::IMPL)) {
            trait_impl = type();
        }

        bool is_method = match(TokenType::FUNC) || (peek().type == TokenType::IDENTIFIER && lookahead().type == TokenType::LEFT_PAREN) || trait_impl != nullptr;

        if (is_method) {
            if (trait_impl) {
                if (auto base_type = dynamic_cast<BaseTypeExpr*>(trait_impl.get())) {
                    traits.push_back(std::make_unique<VariableExpr>(base_type->type));
                }
            }
            methods.push_back(functionDeclaration("method", true, current_access, std::move(trait_impl)));
        } else if (peek().type == TokenType::IDENTIFIER && lookahead().type == TokenType::COLON) {
            fields.push_back(varDeclaration(true, current_access));
        } else {
            error(peek(), "Expect field or method in struct body.");
            advance();
        }
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after struct body.");
    return std::make_unique<StructStmt>(std::move(name), std::move(generic_params), std::move(generic_constraints), std::move(traits), std::move(fields), std::move(methods));
}

std::unique_ptr<Stmt> Parser::enumDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect enum name.");
    consume(TokenType::LEFT_BRACE, "Expect '{' before enum members.");
    std::vector<Token> members;
    if (!check(TokenType::RIGHT_BRACE)) {
        do {
            members.push_back(consume(TokenType::IDENTIFIER, "Expect enum member name."));
        } while (match(TokenType::COMMA));
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after enum members.");
    return std::make_unique<EnumStmt>(std::move(name), std::move(members));
}

std::unique_ptr<Stmt> Parser::traitDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect trait name.");
    consume(TokenType::LEFT_BRACE, "Expect '{' before trait body.");
    std::vector<std::unique_ptr<FunctionStmt>> methods;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        consume(TokenType::FUNC, "Expect 'func' before method signature in trait.");
        methods.push_back(functionDeclaration("method", false));
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after trait body.");
    return std::make_unique<TraitStmt>(std::move(name), std::move(methods));
}

std::vector<std::unique_ptr<Stmt>> Parser::block() {
    std::vector<std::unique_ptr<Stmt>> statements;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        statements.push_back(declaration());
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
    return statements;
}

std::unique_ptr<TypeExpr> Parser::type() {
    if (match(TokenType::AMPERSAND)) {
        bool isMutable = match(TokenType::MUT);
        auto element_type = type();
        return std::make_unique<BorrowTypeExpr>(std::move(element_type), isMutable);
    }
    if (match(TokenType::ARRAY)) {
        consume(TokenType::LEFT_BRACKET, "Expect '[' after 'array'.");
        auto element_type = type();
        std::unique_ptr<Expr> size = nullptr;
        if (match(TokenType::SEMICOLON)) {
            size = expression();
        }
        consume(TokenType::RIGHT_BRACKET, "Expect ']' after array type.");
        return std::make_unique<ArrayTypeExpr>(std::move(element_type), std::move(size));
    }
    if (match(TokenType::IDENTIFIER, TokenType::INT, TokenType::UINT, TokenType::STRING_TYPE, TokenType::BOOL, TokenType::VOID, TokenType::DOUBLE, TokenType::CHAR_TYPE, TokenType::REFLECT, TokenType::OPTION)) {
        Token base_type = previous();
        if (match(TokenType::LESS)) {
            std::vector<std::unique_ptr<TypeExpr>> generic_args;
            do {
                generic_args.push_back(type());
            } while (match(TokenType::COMMA));
            consume(TokenType::GREATER, "Expect '>' after generic arguments.");
            return std::make_unique<GenericTypeExpr>(std::move(base_type), std::move(generic_args));
        }
        return std::make_unique<BaseTypeExpr>(std::move(base_type));
    }

    // For now, we only support base types.
    // We will add array and function types here later.
    throw error(peek(), "Expect type name.");
}


std::unique_ptr<Expr> Parser::expression() {
    return assignment();
}

std::unique_ptr<Expr> Parser::assignment() {
    auto expr = equality();

    if (match(TokenType::EQUAL)) {
        Token equals = previous();
        auto value = assignment();

        if (auto varExpr = dynamic_cast<VariableExpr*>(expr.get())) {
            return std::make_unique<AssignExpr>(varExpr->name, std::move(value));
        } else if (auto getExpr = dynamic_cast<GetExpr*>(expr.get())) {
            return std::make_unique<SetExpr>(std::move(getExpr->object), getExpr->name, std::move(value));
        } else if (auto derefExpr = dynamic_cast<DerefExpr*>(expr.get())) {
            return std::make_unique<SetExpr>(std::move(derefExpr->expression), Token{TokenType::STAR, "*", nullptr, 0}, std::move(value));
        }

        error(equals, "Invalid assignment target.");
    } else if (match(TokenType::PLUS_EQUAL, TokenType::MINUS_EQUAL, TokenType::STAR_EQUAL, TokenType::SLASH_EQUAL, TokenType::PERCENT_EQUAL)) {
        Token op = previous();
        auto value = assignment();
        return std::make_unique<BinaryExpr>(std::move(expr), std::move(op), std::move(value));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::equality() {
    auto expr = comparison();
    while (match(TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL)) {
        Token op = previous();
        auto right = comparison();
        expr = std::make_unique<BinaryExpr>(std::move(expr), std::move(op), std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::comparison() {
    auto expr = term();
    while (match(TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL)) {
        Token op = previous();
        auto right = term();
        expr = std::make_unique<BinaryExpr>(std::move(expr), std::move(op), std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::term() {
    auto expr = factor();
    while (match(TokenType::MINUS, TokenType::PLUS)) {
        Token op = previous();
        auto right = factor();
        expr = std::make_unique<BinaryExpr>(std::move(expr), std::move(op), std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::factor() {
    auto expr = unary();
    while (match(TokenType::SLASH, TokenType::STAR, TokenType::PERCENT)) {
        Token op = previous();
        auto right = unary();
        expr = std::make_unique<BinaryExpr>(std::move(expr), std::move(op), std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::unary() {
    if (match(TokenType::BANG, TokenType::MINUS, TokenType::STAR, TokenType::PLUS_PLUS, TokenType::MINUS_MINUS)) {
        Token op = previous();
        auto right = unary();
        if (op.type == TokenType::STAR) {
            return std::make_unique<DerefExpr>(std::move(right));
        }
        return std::make_unique<UnaryExpr>(std::move(op), std::move(right));
    }
    if (match(TokenType::AMPERSAND)) {
        bool isMutable = match(TokenType::MUT);
        auto expr = unary();
        return std::make_unique<BorrowExpr>(std::move(expr), isMutable);
    }
    return call();
}

std::unique_ptr<Expr> Parser::call() {
    auto expr = primary();
    while (true) {
        if (match(TokenType::LEFT_PAREN)) {
            expr = finishCall(std::move(expr), {});
        } else if (isGenericArgumentList()) {
            advance(); // consume '<'
            std::vector<std::unique_ptr<TypeExpr>> generic_args;
            do {
                generic_args.push_back(type());
            } while (match(TokenType::COMMA));
            consume(TokenType::GREATER, "Expect '>' after generic arguments.");
            consume(TokenType::LEFT_PAREN, "Expect '(' after generic arguments.");
            expr = finishCall(std::move(expr), std::move(generic_args));
        } else if (match(TokenType::COLON_COLON, TokenType::DOT)) {
            Token name = consume(TokenType::IDENTIFIER, "Expect property name after '::' or '.'.");
            expr = std::make_unique<GetExpr>(std::move(expr), std::move(name));
        } else if (match(TokenType::PLUS_PLUS, TokenType::MINUS_MINUS)) {
            Token op = previous();
            expr = std::make_unique<UnaryExpr>(std::move(op), std::move(expr), true);
        } else {
            break;
        }
    }
    return expr;
}

std::unique_ptr<Expr> Parser::finishCall(std::unique_ptr<Expr> callee, std::vector<std::unique_ptr<TypeExpr>> generic_args) {
    std::vector<std::unique_ptr<Expr>> arguments;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            arguments.push_back(expression());
        } while (match(TokenType::COMMA));
    }
    Token paren = consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");
    return std::make_unique<CallExpr>(std::move(callee), std::move(paren), std::move(generic_args), std::move(arguments));
}

std::unique_ptr<Expr> Parser::primary() {
    if (match(TokenType::FALSE)) return std::make_unique<LiteralExpr>(false);
    if (match(TokenType::TRUE)) return std::make_unique<LiteralExpr>(true);
    if (match(TokenType::NONE)) return std::make_unique<LiteralExpr>(nullptr);

    if (match(TokenType::NUMBER_LITERAL, TokenType::STRING_LITERAL, TokenType::CHAR_LITERAL)) {
        return std::make_unique<LiteralExpr>(previous().literal);
    }

    if (match(TokenType::TYPE_CAST)) {
        auto type_cast_expr = std::make_unique<TypeCastExpr>(nullptr, nullptr);
        consume(TokenType::LESS, "Expect '<' after 'type_cast'.");
        type_cast_expr->type = type();
        consume(TokenType::GREATER, "Expect '>' after type in type_cast.");
        consume(TokenType::LEFT_PAREN, "Expect '(' after type_cast<T>.");
        type_cast_expr->expression = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression in type_cast.");
        return type_cast_expr;
    }

    if (match(TokenType::IDENTIFIER, TokenType::PRINT, TokenType::INPUT, TokenType::FS_READ, TokenType::FS_WRITE, TokenType::META, TokenType::OPERATOR, TokenType::REFLECT, TokenType::UTIL)) {
        if (peek().type == TokenType::LEFT_BRACE) {
            return structLiteral();
        }
        return std::make_unique<VariableExpr>(previous());
    }
    if (match(TokenType::SELF)) {
        return std::make_unique<SelfExpr>(previous());
    }

    if (match(TokenType::LEFT_PAREN)) {
        auto expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_unique<GroupingExpr>(std::move(expr));
    }

    if (isLambdaExpressionAhead()) {
        return lambdaExpression();
    }

    if (match(TokenType::LEFT_BRACKET)) {
        std::vector<std::unique_ptr<Expr>> elements;
        if (!check(TokenType::RIGHT_BRACKET)) {
            do {
                elements.push_back(expression());
            } while (match(TokenType::COMMA));
        }
        consume(TokenType::RIGHT_BRACKET, "Expect ']' after array elements.");
        return std::make_unique<ArrayLiteralExpr>(std::move(elements));
    }

    throw error(peek(), "Expect expression.");
}

std::unique_ptr<Expr> Parser::lambdaExpression() {
    consume(TokenType::LEFT_BRACKET, "Expect '[' at the start of a lambda expression.");
    std::vector<Token> captures;
    if (!check(TokenType::RIGHT_BRACKET)) {
        do {
            if (match(TokenType::AMPERSAND)) {
                captures.push_back(previous());
            }
            if (match(TokenType::IDENTIFIER)) {
                captures.push_back(previous());
            }
        } while (match(TokenType::COMMA));
    }
    consume(TokenType::RIGHT_BRACKET, "Expect ']' after lambda capture list.");

    consume(TokenType::LEFT_PAREN, "Expect '(' after lambda capture list.");
    std::vector<Token> parameters;
    std::vector<std::unique_ptr<TypeExpr>> param_types;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            parameters.push_back(consume(TokenType::IDENTIFIER, "Expect parameter name."));
            consume(TokenType::COLON, "Expect ':' after parameter name.");
            param_types.push_back(type());
        } while (match(TokenType::COMMA));
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");

    std::unique_ptr<TypeExpr> returnType = nullptr;
    if (match(TokenType::ARROW)) {
        returnType = type();
    }

    consume(TokenType::LEFT_BRACE, "Expect '{' before lambda body.");
    auto body = std::make_unique<BlockStmt>(block());

    return std::make_unique<LambdaExpr>(std::move(captures), std::move(parameters), std::move(param_types), std::move(returnType), std::move(body));
}

std::unique_ptr<Expr> Parser::structLiteral() {
    Token name = previous();
    consume(TokenType::LEFT_BRACE, "Expect '{' after struct name.");
    std::map<std::string, std::unique_ptr<Expr>> fields;
    if (!check(TokenType::RIGHT_BRACE)) {
        do {
            Token field_name = consume(TokenType::IDENTIFIER, "Expect field name.");
            consume(TokenType::COLON, "Expect ':' after field name.");
            fields[field_name.lexeme] = expression();
        } while (match(TokenType::COMMA));
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after struct fields.");
    return std::make_unique<StructLiteralExpr>(std::move(name), std::move(fields));
}

// Helper method implementations
bool Parser::isAtEnd() {
    return peek().type == TokenType::END_OF_FILE;
}

Token Parser::peek() {
    return tokens[current];
}

Token Parser::lookahead() {
    if (isAtEnd()) return peek();
    return tokens[current + 1];
}

Token Parser::previous() {
    return tokens[current - 1];
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

template<typename... Args>
bool Parser::match(Args... types) {
    for (TokenType type : {types...}) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw error(peek(), message);
}

Parser::ParseError Parser::error(const Token& token, const std::string& message) {
    std::cerr << "ParseError at token " << token.lexeme << ": " << message << std::endl;
    return ParseError(message);
}

void Parser::synchronize() {
    advance();

    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return;

        switch (peek().type) {
            case TokenType::FUNC:
            case TokenType::LET:
            case TokenType::MUT:
            case TokenType::FOR:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::RETURN:
                return;
        }

        advance();
    }
}

bool Parser::isGenericArgumentList() {
    if (peek().type != TokenType::LESS) {
        return false;
    }

    int saved_current = current;
    bool result = false;

    try {
        advance(); // consume '<'
        if (isAtEnd()) {
            current = saved_current;
            return false;
        }

        // Check for at least one type
        type();

        while (match(TokenType::COMMA)) {
            type();
        }

        if (peek().type == TokenType::GREATER) {
            advance(); // consume '>'
            if (!isAtEnd() && peek().type == TokenType::LEFT_PAREN) {
                result = true;
            }
        }
    } catch (ParseError& e) {
        // Fallthrough, result is false
    }

    current = saved_current;
    return result;
}

bool Parser::isLambdaExpressionAhead() {
    if (peek().type != TokenType::LEFT_BRACKET) {
        return false;
    }

    int saved_current = current;
    advance(); // Consume '['

    // Look for ']'
    while (!isAtEnd() && peek().type != TokenType::RIGHT_BRACKET) {
        advance();
    }

    if (isAtEnd() || peek().type != TokenType::RIGHT_BRACKET) {
        current = saved_current;
        return false;
    }

    advance(); // Consume ']'

    bool result = !isAtEnd() && peek().type == TokenType::LEFT_PAREN;

    current = saved_current;
    return result;
}

} // namespace chtholly
