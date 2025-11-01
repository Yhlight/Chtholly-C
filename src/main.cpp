#include <iostream>
#include <vector>
#include <string>
#include "../include/Lexer.h"
#include "../include/Parser.h"
#include "../include/CodeGen.h"

int main() {
    std::string source = R"(
        let a = 10;
        mut b = "hello";
        let c = a + 5;
        b = "world";
    )";

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::shared_ptr<Stmt>> statements = parser.parse();

    CodeGen generator;
    std::string generated_code = generator.generate(statements);

    std::cout << "// Generated C++ Code:\n";
    std::cout << generated_code;

    return 0;
}
