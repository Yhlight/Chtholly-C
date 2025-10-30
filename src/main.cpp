#include <iostream>
#include "Lexer.h"
#include "Parser.h"
#include "AST/ASTPrinter.h"

int main() {
    std::string source = "let x = 10; let y = 20;";
    Lexer lexer(source);
    Parser parser(lexer);
    auto statements = parser.parse();

    ASTPrinter printer;
    std::string result = printer.print(statements);
    std::cout << result;

    return 0;
}
