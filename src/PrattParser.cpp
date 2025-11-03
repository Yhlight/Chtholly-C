#include "PrattParser.h"
#include "Parser.h"
#include "Error.h"

PrattParser::PrattParser(Parser& parser) : parser(parser) {
    rules = {
        {TokenType::LEFT_PAREN, {std::bind(&PrattParser::grouping, this), std::bind(&PrattParser::call, this, std::placeholders::_1), Precedence::CALL}},
        {TokenType::DOT, {nullptr, std::bind(&PrattParser::get, this, std::placeholders::_1), Precedence::CALL}},
        {TokenType::MINUS, {std::bind(&PrattParser::unary, this), std::bind(&PrattParser::binary, this, std::placeholders::_1), Precedence::TERM}},
        {TokenType::PLUS, {nullptr, std::bind(&PrattParser::binary, this, std::placeholders::_1), Precedence::TERM}},
        {TokenType::SLASH, {nullptr, std::bind(&PrattParser::binary, this, std::placeholders::_1), Precedence::FACTOR}},
        {TokenType::STAR, {nullptr, std::bind(&PrattParser::binary, this, std::placeholders::_1), Precedence::FACTOR}},
        {TokenType::BANG, {std::bind(&PrattParser::unary, this), nullptr, Precedence::NONE}},
        {TokenType::BANG_EQUAL, {nullptr, std::bind(&PrattParser::binary, this, std::placeholders::_1), Precedence::EQUALITY}},
        {TokenType::EQUAL, {nullptr, std::bind(&PrattParser::assignment, this, std::placeholders::_1), Precedence::ASSIGNMENT}},
        {TokenType::EQUAL_EQUAL, {nullptr, std::bind(&PrattParser::binary, this, std::placeholders::_1), Precedence::EQUALITY}},
        {TokenType::GREATER, {nullptr, std::bind(&PrattParser::binary, this, std::placeholders::_1), Precedence::COMPARISON}},
        {TokenType::GREATER_EQUAL, {nullptr, std::bind(&PrattParser::binary, this, std::placeholders::_1), Precedence::COMPARISON}},
        {TokenType::LESS, {nullptr, std::bind(&PrattParser::binary, this, std::placeholders::_1), Precedence::COMPARISON}},
        {TokenType::LESS_EQUAL, {nullptr, std::bind(&PrattParser::binary, this, std::placeholders::_1), Precedence::COMPARISON}},
        {TokenType::PIPE_PIPE, {nullptr, std::bind(&PrattParser::binary, this, std::placeholders::_1), Precedence::OR}},
        {TokenType::AMPERSAND_AMPERSAND, {nullptr, std::bind(&PrattParser::binary, this, std::placeholders::_1), Precedence::AND}},
        {TokenType::AMPERSAND, {std::bind(&PrattParser::borrow, this), nullptr, Precedence::NONE}},
        {TokenType::LEFT_BRACKET, {std::bind(&PrattParser::lambda, this), nullptr, Precedence::NONE}},
        {TokenType::IDENTIFIER, {std::bind(&PrattParser::variable, this), nullptr, Precedence::NONE}},
        {TokenType::STRING, {std::bind(&PrattParser::literal, this), nullptr, Precedence::NONE}},
        {TokenType::NUMBER, {std::bind(&PrattParser::literal, this), nullptr, Precedence::NONE}},
        {TokenType::TRUE, {std::bind(&PrattParser::literal, this), nullptr, Precedence::NONE}},
        {TokenType::FALSE, {std::bind(&PrattParser::literal, this), nullptr, Precedence::NONE}},
        {TokenType::LEFT_BRACE, {nullptr, std::bind(&PrattParser::structInitializer, this, std::placeholders::_1), Precedence::CALL}},
    };
}

std::unique_ptr<Expr> PrattParser::parse() {
    return parsePrecedence(Precedence::ASSIGNMENT);
}

std::unique_ptr<Expr> PrattParser::parsePrecedence(Precedence precedence) {
    auto token_type = parser.peek().type;
    auto prefixRule = getRule(token_type).prefix;

    if (prefixRule == nullptr) {
        ErrorReporter::error(parser.peek().line, "Expect expression.");
        return nullptr;
    }
    parser.advance();
    auto left = prefixRule();

    while (precedence <= getRule(parser.peek().type).precedence) {
        token_type = parser.peek().type;
        if (token_type == TokenType::LESS && LA_is_generic_call()) {
            parser.advance();
            left = genericCall(std::move(left));
        } else {
            auto infixRule = getRule(token_type).infix;
            if (infixRule == nullptr) {
                break;
            }
            parser.advance();
            left = infixRule(std::move(left));
        }
    }

    return left;
}

const PrattParser::ParseRule& PrattParser::getRule(TokenType type) {
    if (rules.find(type) == rules.end()) {
        static ParseRule null_rule = {nullptr, nullptr, Precedence::NONE};
        return null_rule;
    }
    return rules.at(type);
}

bool PrattParser::LA_is_generic_call() {
    int saved = parser.current;
    try {
        if (!parser.match({TokenType::LESS})) {
            parser.current = saved;
            return false;
        }
        do {
            parser.parseType();
        } while (parser.match({TokenType::COMMA}));
        if (parser.match({TokenType::GREATER})) {
            parser.current = saved;
            return true;
        }
    } catch (Parser::ParseError& error) {
        // fall through
    }
    parser.current = saved;
    return false;
}

std::unique_ptr<Expr> PrattParser::unary() {
    Token op = parser.previous();
    auto right = parsePrecedence(Precedence::UNARY);
    return std::make_unique<UnaryExpr>(op, std::move(right));
}

std::unique_ptr<Expr> PrattParser::binary(std::unique_ptr<Expr> left) {
    Token op = parser.previous();
    const auto& rule = getRule(op.type);
    auto right = parsePrecedence((Precedence)((int)rule.precedence + 1));
    return std::make_unique<BinaryExpr>(std::move(left), op, std::move(right));
}

std::unique_ptr<Expr> PrattParser::grouping() {
    auto expr = parse();
    parser.consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
    return std::make_unique<GroupingExpr>(std::move(expr));
}

std::unique_ptr<Expr> PrattParser::literal() {
    switch (parser.previous().type) {
        case TokenType::TRUE: return std::make_unique<LiteralExpr>(true);
        case TokenType::FALSE: return std::make_unique<LiteralExpr>(false);
        default: return std::make_unique<LiteralExpr>(parser.previous().literal);
    }
}

std::unique_ptr<Expr> PrattParser::variable() {
    return std::make_unique<VariableExpr>(parser.previous());
}

std::unique_ptr<Expr> PrattParser::call(std::unique_ptr<Expr> callee) {
    return finishCall(std::move(callee), {});
}

std::unique_ptr<Expr> PrattParser::genericCall(std::unique_ptr<Expr> callee) {
    std::vector<TypeInfo> generic_args;
    do {
        generic_args.push_back(parser.parseType());
    } while (parser.match({TokenType::COMMA}));
    parser.consume(TokenType::GREATER, "Expect '>' after generic arguments.");
    parser.consume(TokenType::LEFT_PAREN, "Expect '(' after generic arguments.");
    return finishCall(std::move(callee), std::move(generic_args));
}

std::unique_ptr<Expr> PrattParser::finishCall(std::unique_ptr<Expr> callee, std::vector<TypeInfo> generic_args) {
    std::vector<std::unique_ptr<Expr>> arguments;
    if (parser.peek().type != TokenType::RIGHT_PAREN) {
        do {
            if (arguments.size() >= 255) {
                ErrorReporter::error(parser.peek().line, "Can't have more than 255 arguments.");
            }
            arguments.push_back(parse());
        } while (parser.match({TokenType::COMMA}));
    }

    Token paren = parser.consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");

    return std::make_unique<CallExpr>(std::move(callee), paren, std::move(arguments), std::move(generic_args));
}

std::unique_ptr<Expr> PrattParser::get(std::unique_ptr<Expr> left) {
    Token name = parser.consume(TokenType::IDENTIFIER, "Expect property name after '.'.");
    return std::make_unique<GetExpr>(std::move(left), name);
}

std::unique_ptr<Expr> PrattParser::assignment(std::unique_ptr<Expr> left) {
    auto value = parsePrecedence(Precedence::ASSIGNMENT);

    if (auto* var = dynamic_cast<VariableExpr*>(left.get())) {
        return std::make_unique<AssignExpr>(var->name, std::move(value));
    } else if (auto* get = dynamic_cast<GetExpr*>(left.get())) {
        return std::make_unique<SetExpr>(std::move(get->object), get->name, std::move(value));
    }

    ErrorReporter::error(parser.previous().line, "Invalid assignment target.");
    return nullptr;
}

std::unique_ptr<Expr> PrattParser::borrow() {
    bool isMutable = parser.match({TokenType::MUT});
    auto expr = parsePrecedence(Precedence::UNARY);
    return std::make_unique<BorrowExpr>(std::move(expr), isMutable);
}

std::unique_ptr<Expr> PrattParser::lambda() {
    parser.consume(TokenType::RIGHT_BRACKET, "Expect ']' after lambda capture list.");
    parser.consume(TokenType::LEFT_PAREN, "Expect '(' after lambda capture list.");
    auto parameters = parser.parseParameters();
    parser.consume(TokenType::RIGHT_PAREN, "Expect ')' after lambda parameters.");
    auto returnType = parser.parseReturnType();
    parser.consume(TokenType::LEFT_BRACE, "Expect '{' before lambda body.");
    auto body = parser.block();
    return std::make_unique<LambdaExpr>(std::move(parameters), std::move(body), std::move(returnType));
}

std::unique_ptr<Expr> PrattParser::structInitializer(std::unique_ptr<Expr> left) {
    if (auto* var = dynamic_cast<VariableExpr*>(left.get())) {
        std::map<Token, std::unique_ptr<Expr>> initializers;
        if (parser.peek().type != TokenType::RIGHT_BRACE) {
            do {
                Token name = parser.consume(TokenType::IDENTIFIER, "Expect field name.");
                parser.consume(TokenType::COLON, "Expect ':' after field name.");
                initializers[name] = parse();
            } while (parser.match({TokenType::COMMA}));
        }
        parser.consume(TokenType::RIGHT_BRACE, "Expect '}' after struct initializer.");
        return std::make_unique<StructInitializerExpr>(var->name, std::move(initializers));
    }
    ErrorReporter::error(parser.peek().line, "Expect struct name before initializer.");
    return nullptr;
}
