#include "../src/Parser.h"
#include "../src/Lexer.h"
#include <iostream>

void test_parser() {
    std::string source = "let x = 10 + 20;";
    Lexer lexer(source);
    Parser parser(lexer);

    auto ast = parser.parse_expression();

    if (ast == nullptr) {
        std::cerr << "Test failed: Parser returned a null AST" << std::endl;
        exit(1);
    }

    auto var_decl = dynamic_cast<VarDeclAST*>(ast.get());
    if (var_decl == nullptr) {
        std::cerr << "Test failed: Expected a VarDeclAST" << std::endl;
        exit(1);
    }

    if (var_decl->get_name() != "x") {
        std::cerr << "Test failed: Expected variable name 'x'" << std::endl;
        exit(1);
    }

    auto bin_expr = dynamic_cast<BinaryExprAST*>(var_decl->get_init());
    if (bin_expr == nullptr) {
        std::cerr << "Test failed: Expected a BinaryExprAST" << std::endl;
        exit(1);
    }

    if (bin_expr->get_op() != '+') {
        std::cerr << "Test failed: Expected operator '+'" << std::endl;
        exit(1);
    }

    auto lhs = dynamic_cast<NumberExprAST*>(bin_expr->get_lhs());
    if (lhs == nullptr || lhs->get_val() != 10) {
        std::cerr << "Test failed: Expected LHS to be 10" << std::endl;
        exit(1);
    }

    auto rhs = dynamic_cast<NumberExprAST*>(bin_expr->get_rhs());
    if (rhs == nullptr || rhs->get_val() != 20) {
        std::cerr << "Test failed: Expected RHS to be 20" << std::endl;
        exit(1);
    }

    std::cout << "Parser test passed!" << std::endl;
}

void test_array_type_parsing() {
    std::string source = "let x: array[array[int; 5]; 10] = 1;";
    Lexer lexer(source);
    Parser parser(lexer);

    auto ast = parser.parse_expression();

    if (ast == nullptr) {
        std::cerr << "Test failed: Parser returned a null AST for array type" << std::endl;
        exit(1);
    }

    auto var_decl = dynamic_cast<VarDeclAST*>(ast.get());
    if (var_decl == nullptr) {
        std::cerr << "Test failed: Expected a VarDeclAST for array type" << std::endl;
        exit(1);
    }

    auto array_type = std::dynamic_pointer_cast<ArrayType>(var_decl->type);
    if (array_type == nullptr) {
        std::cerr << "Test failed: Expected an ArrayType" << std::endl;
        exit(1);
    }

    if (array_type->get_size() != 10) {
        std::cerr << "Test failed: Expected array size 10" << std::endl;
        exit(1);
    }

    auto element_type = std::dynamic_pointer_cast<ArrayType>(array_type->get_element_type());
    if (element_type == nullptr) {
        std::cerr << "Test failed: Expected an ArrayType as element type" << std::endl;
        exit(1);
    }

    if (element_type->get_size() != 5) {
        std::cerr << "Test failed: Expected nested array size 5" << std::endl;
        exit(1);
    }

    auto nested_element_type = std::dynamic_pointer_cast<IntType>(element_type->get_element_type());
    if (nested_element_type == nullptr) {
        std::cerr << "Test failed: Expected an IntType as nested element type" << std::endl;
        exit(1);
    }

    std::cout << "Array type parsing test passed!" << std::endl;
}

void test_if_else_parsing() {
    std::string source = "if (1 < 2) { let x = 1; } else { let y = 2; }";
    Lexer lexer(source);
    Parser parser(lexer);

    auto ast = parser.parse_expression();

    if (ast == nullptr) {
        std::cerr << "Test failed: Parser returned a null AST for if-else" << std::endl;
        exit(1);
    }

    auto if_expr = dynamic_cast<IfExprAST*>(ast.get());
    if (if_expr == nullptr) {
        std::cerr << "Test failed: Expected an IfExprAST" << std::endl;
        exit(1);
    }

    auto cond = dynamic_cast<BinaryExprAST*>(if_expr->get_cond());
    if (cond == nullptr || cond->get_op() != '<') {
        std::cerr << "Test failed: Expected a BinaryExprAST with '<' operator for condition" << std::endl;
        exit(1);
    }

    auto then_block = dynamic_cast<BlockExprAST*>(if_expr->get_then());
    if (then_block == nullptr || then_block->get_expressions().size() != 1) {
        std::cerr << "Test failed: Expected a BlockExprAST with one expression for then block" << std::endl;
        exit(1);
    }

    auto else_block = dynamic_cast<BlockExprAST*>(if_expr->get_else());
    if (else_block == nullptr || else_block->get_expressions().size() != 1) {
        std::cerr << "Test failed: Expected a BlockExprAST with one expression for else block" << std::endl;
        exit(1);
    }

    std::cout << "If-else parsing test passed!" << std::endl;
}

int main() {
    test_parser();
    test_array_type_parsing();
    test_if_else_parsing();
    return 0;
}
