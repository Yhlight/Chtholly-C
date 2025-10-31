#pragma once

#include "AST.h"
#include "Lexer.h"
#include <memory>
#include <vector>

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
    std::unique_ptr<Expression> parse_expression();
    std::unique_ptr<Expression> parse_integer_literal();

    Lexer& lexer;
    Token current_token;
    Token peek_token;
    std::vector<std::string> error_messages;
};
