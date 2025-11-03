#include <iostream>
#include <vector>
#include "Lexer.h"
#include "Parser.h"
#include "AstPrinter.h"

void run(const std::string& source) {
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::vector<std::shared_ptr<Stmt>> statements = parser.parse();

    for (const auto& stmt : statements) {
        if (stmt) {
            AstPrinter printer;
            std::cout << printer.print(stmt) << std::endl;
        }
    }
}

int main() {
    std::string source = R"(
        let a = 1;
        a = a + 2;
        print a;
    )";
    run(source);
    return 0;
}
