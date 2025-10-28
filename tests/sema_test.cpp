#include "../src/Sema.h"
#include "../src/Parser.h"
#include "../src/Lexer.h"
#include <iostream>

void test_sema() {
    // Test use of moved variable
    try {
        std::string source = "let x = 10; let y = x; let z = x;";
        Lexer lexer(source);
        Parser parser(lexer);
        Sema sema;
        sema.treat_int_as_move = true;
        auto ast = parser.parse_block();
        sema.visit(*ast);
        std::cerr << "Test failed: Expected an error for use of moved variable" << std::endl;
        exit(1);
    } catch (const std::runtime_error& e) {
        std::string error = e.what();
        if (error.find("Use of moved variable") == std::string::npos) {
            std::cerr << "Test failed: Unexpected error message: " << error << std::endl;
            exit(1);
        }
    }

    // Test assignment to moved variable
    try {
        std::string source = "let x = 10; let y = x; x = 20;";
        Lexer lexer(source);
        Parser parser(lexer);
        Sema sema;
        sema.treat_int_as_move = true;
        auto ast = parser.parse_block();
        sema.visit(*ast);
        std::cerr << "Test failed: Expected an error for assignment to moved variable" << std::endl;
        exit(1);
    } catch (const std::runtime_error& e) {
        std::string error = e.what();
        if (error.find("Assignment to moved variable") == std::string::npos) {
            std::cerr << "Test failed: Unexpected error message: " << error << std::endl;
            exit(1);
        }
    }

    std::cout << "Sema test passed!" << std::endl;
}

int main() {
    test_sema();
    return 0;
}
