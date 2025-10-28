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

    std::cout << "Parser test passed!" << std::endl;
}

int main() {
    test_parser();
    return 0;
}
