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

int main() {
    test_parser();
    return 0;
}
