#pragma once

#include "AST.h"
#include "Lexer.h"
#include <memory>
#include <vector>
#include <functional>
#include <map>

enum Precedence {
    LOWEST,
    EQUALS,      // ==
    LESSGREATER, // > or <
    SUM,         // +
    PRODUCT,     // *
    PREFIX,      // -X or !X
    CALL,        // myFunction(X)
};

class Parser {
public:
    explicit Parser(Lexer& lexer);
    std::unique_ptr<Program> parse_program();
    const std::vector<std::string>& errors() const;

private:
    void next_token();
    std::unique_ptr<Statement> parse_statement();
    std::unique_ptr<LetStatement> parse_let_statement();
    std::unique_ptr<MutStatement> parse_mut_statement();
    std::unique_ptr<Expression> parse_expression(Precedence precedence);
    std::unique_ptr<Expression> parse_integer_literal();
    std::unique_ptr<FunctionStatement> parse_function_statement();
    std::unique_ptr<BlockStatement> parse_block_statement();
    std::unique_ptr<Expression> parse_prefix_expression();
    std::unique_ptr<Expression> parse_infix_expression(std::unique_ptr<Expression> left);
    std::unique_ptr<Statement> parse_expression_statement();

    Lexer& lexer;
    Token current_token;
    Token peek_token;
    std::vector<std::string> error_messages;

    using prefix_parse_fn = std::function<std::unique_ptr<Expression>()>;
    using infix_parse_fn = std::function<std::unique_ptr<Expression>(std::unique_ptr<Expression>)>;

    std::map<TokenType, prefix_parse_fn> prefix_parse_fns;
    std::map<TokenType, infix_parse_fn> infix_parse_fns;
    std::map<TokenType, Precedence> precedences;
};
