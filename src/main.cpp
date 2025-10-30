#include <iostream>
#include "Lexer.h"
#include "Parser.h"
#include "Resolver.h"
#include "Transpiler.h"

int main() {
    std::string source = "func add(a: int, b: int) -> int { return a + b; }";
    try {
        Lexer lexer(source);
        Parser parser(lexer);
        auto statements = parser.parse();

        Resolver resolver;
        resolver.resolve(statements);

        Transpiler transpiler;
        std::string result = transpiler.transpile(statements);
        std::cout << result;
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
