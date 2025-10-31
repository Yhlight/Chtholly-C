#include "Parser.h"
#include <stdexcept>

Parser::Parser(Lexer& lexer) : lexer(lexer) {
    next_token();
    next_token();

    // Register prefix parsing functions
    prefix_parse_fns[TokenType::Identifier] = [this] { return std::make_unique<Identifier>(current_token.literal); };
    prefix_parse_fns[TokenType::Integer] = [this] { return parse_integer_literal(); };
    prefix_parse_fns[TokenType::LogicalNot] = [this] { return parse_prefix_expression(); };
    prefix_parse_fns[TokenType::Minus] = [this] { return parse_prefix_expression(); };

    // Register infix parsing functions
    infix_parse_fns[TokenType::Plus] = [this](auto left) { return parse_infix_expression(std::move(left)); };
    infix_parse_fns[TokenType::Minus] = [this](auto left) { return parse_infix_expression(std::move(left)); };
    infix_parse_fns[TokenType::Slash] = [this](auto left) { return parse_infix_expression(std::move(left)); };
    infix_parse_fns[TokenType::Asterisk] = [this](auto left) { return parse_infix_expression(std::move(left)); };
    infix_parse_fns[TokenType::DoubleEqual] = [this](auto left) { return parse_infix_expression(std::move(left)); };
    infix_parse_fns[TokenType::NotEqual] = [this](auto left) { return parse_infix_expression(std::move(left)); };
    infix_parse_fns[TokenType::LessThan] = [this](auto left) { return parse_infix_expression(std::move(left)); };
    infix_parse_fns[TokenType::GreaterThan] = [this](auto left) { return parse_infix_expression(std::move(left)); };

    // Define operator precedences
    precedences[TokenType::DoubleEqual] = Precedence::EQUALS;
    precedences[TokenType::NotEqual] = Precedence::EQUALS;
    precedences[TokenType::LessThan] = Precedence::LESSGREATER;
    precedences[TokenType::GreaterThan] = Precedence::LESSGREATER;
    precedences[TokenType::Plus] = Precedence::SUM;
    precedences[TokenType::Minus] = Precedence::SUM;
    precedences[TokenType::Slash] = Precedence::PRODUCT;
    precedences[TokenType::Asterisk] = Precedence::PRODUCT;
}

std::unique_ptr<Program> Parser::parse_program() {
    auto program = std::make_unique<Program>();

    while (current_token.type != TokenType::Eof) {
        auto stmt = parse_statement();
        if (stmt) {
            program->statements.push_back(std::move(stmt));
        }
        next_token();
    }

    return program;
}

const std::vector<std::string>& Parser::errors() const {
    return error_messages;
}

void Parser::next_token() {
    current_token = peek_token;
    peek_token = lexer.next_token();
}

std::unique_ptr<Statement> Parser::parse_statement() {
    if (current_token.type == TokenType::Let) {
        return parse_let_statement();
    }
    if (current_token.type == TokenType::Mut) {
        return parse_mut_statement();
    }
    if (current_token.type == TokenType::Func) {
        return parse_function_statement();
    }
    return parse_expression_statement();
}

std::unique_ptr<Expression> Parser::parse_expression(Precedence precedence) {
    auto prefix = prefix_parse_fns[current_token.type];
    if (prefix == nullptr) {
        error_messages.push_back("No prefix parse function for " + current_token.literal);
        return nullptr;
    }
    auto left_exp = prefix();

    while (peek_token.type != TokenType::Semicolon && precedence < precedences[peek_token.type]) {
        auto infix = infix_parse_fns[peek_token.type];
        if (infix == nullptr) {
            return left_exp;
        }
        next_token();
        left_exp = infix(std::move(left_exp));
    }

    return left_exp;
}

std::unique_ptr<Expression> Parser::parse_integer_literal() {
    auto literal = std::make_unique<IntegerLiteral>();
    try {
        literal->value = std::stoll(current_token.literal);
    } catch (const std::invalid_argument& e) {
        error_messages.push_back("Could not parse integer literal: " + current_token.literal);
        return nullptr;
    }
    return literal;
}

std::unique_ptr<Expression> Parser::parse_prefix_expression() {
    auto expression = std::make_unique<PrefixExpression>();
    expression->op = current_token.literal;
    next_token();
    expression->right = parse_expression(Precedence::PREFIX);
    return expression;
}

std::unique_ptr<Expression> Parser::parse_infix_expression(std::unique_ptr<Expression> left) {
    auto expression = std::make_unique<InfixExpression>();
    expression->left = std::move(left);
    expression->op = current_token.literal;
    auto precedence = precedences[current_token.type];
    next_token();
    expression->right = parse_expression(precedence);
    return expression;
}

std::unique_ptr<Statement> Parser::parse_expression_statement() {
    auto stmt = std::make_unique<ExpressionStatement>();
    stmt->expression = parse_expression(Precedence::LOWEST);

    if (peek_token.type == TokenType::Semicolon) {
        next_token();
    }

    return stmt;
}

std::unique_ptr<LetStatement> Parser::parse_let_statement() {
    auto stmt = std::make_unique<LetStatement>();

    next_token(); // consume 'let'

    if (current_token.type != TokenType::Identifier) {
        error_messages.push_back("Expected identifier after 'let'");
        return nullptr;
    }

    stmt->name = std::make_unique<Identifier>(current_token.literal);

    next_token(); // consume identifier

    if (current_token.type == TokenType::Colon) {
        next_token(); // consume ':'
        stmt->type = std::make_unique<Type>(current_token.literal);
        next_token(); // consume type
    }

    if (current_token.type != TokenType::Equal) {
        error_messages.push_back("Expected '=' after identifier");
        return nullptr;
    }

    next_token(); // consume '='

    stmt->value = parse_expression(Precedence::LOWEST);

    if (peek_token.type == TokenType::Semicolon) {
        next_token();
    }

    return stmt;
}

std::unique_ptr<MutStatement> Parser::parse_mut_statement() {
    auto stmt = std::make_unique<MutStatement>();

    next_token(); // consume 'mut'

    if (current_token.type != TokenType::Identifier) {
        error_messages.push_back("Expected identifier after 'mut'");
        return nullptr;
    }

    stmt->name = std::make_unique<Identifier>(current_token.literal);

    next_token(); // consume identifier

    if (current_token.type == TokenType::Colon) {
        next_token(); // consume ':'
        stmt->type = std::make_unique<Type>(current_token.literal);
        next_token(); // consume type
    }

    if (current_token.type != TokenType::Equal) {
        error_messages.push_back("Expected '=' after identifier");
        return nullptr;
    }

    next_token(); // consume '='

    stmt->value = parse_expression(Precedence::LOWEST);

    if (peek_token.type == TokenType::Semicolon) {
        next_token();
    }

    return stmt;
}

std::unique_ptr<FunctionStatement> Parser::parse_function_statement() {
    auto stmt = std::make_unique<FunctionStatement>();

    next_token(); // consume 'func'

    if (current_token.type != TokenType::Identifier) {
        error_messages.push_back("Expected function name");
        return nullptr;
    }

    stmt->name = std::make_unique<Identifier>(current_token.literal);

    next_token(); // consume function name

    if (current_token.type != TokenType::LParen) {
        error_messages.push_back("Expected '(' after function name");
        return nullptr;
    }

    next_token(); // consume '('

    // Parse parameters
    while (current_token.type != TokenType::RParen) {
        if (current_token.type != TokenType::Identifier) {
            error_messages.push_back("Expected parameter name");
            return nullptr;
        }
        auto param_name = std::make_unique<Identifier>(current_token.literal);
        next_token(); // consume parameter name

        if (current_token.type != TokenType::Colon) {
            error_messages.push_back("Expected ':' after parameter name");
            return nullptr;
        }
        next_token(); // consume ':'

        auto param_type = std::make_unique<Type>(current_token.literal);
        next_token(); // consume parameter type

        stmt->parameters.emplace_back(std::move(param_name), std::move(param_type));

        if (current_token.type == TokenType::Comma) {
            next_token(); // consume ','
        }
    }

    next_token(); // consume ')'

    if (current_token.type == TokenType::Minus && peek_token.type == TokenType::GreaterThan) {
        next_token(); // consume '-'
        next_token(); // consume '>'
        stmt->return_type = std::make_unique<Type>(current_token.literal);
        next_token(); // consume return type
    }

    stmt->body = parse_block_statement();

    return stmt;
}

std::unique_ptr<BlockStatement> Parser::parse_block_statement() {
    auto block = std::make_unique<BlockStatement>();

    if (current_token.type != TokenType::LBrace) {
        error_messages.push_back("Expected '{' to start a block statement");
        return nullptr;
    }

    next_token(); // consume '{'

    while (current_token.type != TokenType::RBrace && current_token.type != TokenType::Eof) {
        auto stmt = parse_statement();
        if (stmt) {
            block->statements.push_back(std::move(stmt));
        }
        next_token();
    }

    return block;
}
