#include <iostream>
#include "Lexer.h"
#include "Parser.h"
#include "Transpiler.h"

int main() {
    std::string source = "let x = (10 + 20) * -30;";
    Lexer lexer(source);
    Parser parser(lexer);
    auto statements = parser.parse();

    Transpiler transpiler;
    std::string result = transpiler.transpile(statements);
    std::cout << result;

    return 0;
}
