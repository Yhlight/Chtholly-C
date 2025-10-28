#include "../src/Sema.h"
#include "../src/Parser.h"
#include "../src/Lexer.h"
#include <iostream>

void test_type_system() {
    // Test type inference
    try {
        std::string source = "let x = 10;";
        Lexer lexer(source);
        Parser parser(lexer);
        Sema sema;
        auto ast = parser.parse_block();
        sema.visit(*ast);
        auto var_decl = dynamic_cast<VarDeclAST*>(ast->get_expressions()[0].get());
        if (var_decl->type->to_string() != "int") {
            std::cerr << "Test failed: Type inference failed" << std::endl;
            exit(1);
        }
    } catch (const std::runtime_error& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        exit(1);
    }

    // Test type mismatch
    try {
        std::string source = "let x : string = 10;";
        Lexer lexer(source);
        Parser parser(lexer);
        Sema sema;
        auto ast = parser.parse_block();
        sema.visit(*ast);
        std::cerr << "Test failed: Expected a type mismatch error" << std::endl;
        exit(1);
    } catch (const std::runtime_error& e) {
        std::string error = e.what();
        if (error.find("Initializer type does not match variable type") == std::string::npos) {
            std::cerr << "Test failed: Unexpected error message: " << error << std::endl;
            exit(1);
        }
    }

    std::cout << "Type system test passed!" << std::endl;
}

int main() {
    test_type_system();
    return 0;
}
