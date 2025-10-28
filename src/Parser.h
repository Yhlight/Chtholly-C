#pragma once

#include "AST.h"
#include "Lexer.h"
#include <map>

class Parser {
public:
    Parser(Lexer& lexer);

    std::unique_ptr<ExprAST> parse_expression();
    std::unique_ptr<PrototypeAST> parse_prototype();
    std::unique_ptr<FunctionAST> parse_definition();
    std::unique_ptr<ExprAST> parse_primary();
    std::unique_ptr<ExprAST> parse_variable_declaration();
    std::unique_ptr<ExprAST> parse_number_expression();
    std::unique_ptr<ExprAST> parse_paren_expression();
    std::unique_ptr<ExprAST> parse_identifier_expression();
    std::unique_ptr<BlockExprAST> parse_block();
    std::shared_ptr<Type> parse_type();

private:
    void get_next_token();
    Token get_current_token() const { return current_token_; }
    int get_token_precedence();
    std::unique_ptr<ExprAST> parse_bin_op_rhs(int expr_prec,
                                                std::unique_ptr<ExprAST> lhs);

    Lexer& lexer_;
    Token current_token_;
    static std::map<char, int> bin_op_precedence_;
};
