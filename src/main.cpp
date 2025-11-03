#include <iostream>
#include <vector>
#include "Lexer.h"
#include "Parser.h"
#include "SemanticAnalyzer.h"

void run(const std::string& source) {
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::vector<std::shared_ptr<Stmt>> statements = parser.parse();

    SemanticAnalyzer analyzer;
    analyzer.analyze(statements);

    const auto& errors = analyzer.getErrors();
    if (!errors.empty()) {
        for (const auto& error : errors) {
            std::cout << error << std::endl;
        }
    } else {
        std::cout << "No semantic errors found." << std::endl;
    }
    std::cout << "--------------------" << std::endl;
}

int main() {
    // Test 1: Valid code
    run(R"(
        let a = 1;
        mut b = 2;
        b = a + b;
        print b;
    )");

    // Test 2: Assign to immutable variable
    run(R"(
        let a = 1;
        a = 2;
    )");

    // Test 3: Use of undeclared variable
    run(R"(
        let a = b;
    )");

    return 0;
}
