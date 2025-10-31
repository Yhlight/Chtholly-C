#include "Parser.h"
#include <iostream>

int main() {
    std::string source = "func add(x : int, y : int) -> int { x + y; }";
    Lexer lexer(source);
    Parser parser(lexer);

    auto program = parser.parse_program();

    if (!parser.errors().empty()) {
        for (const auto& error : parser.errors()) {
            std::cerr << "Parser error: " << error << std::endl;
        }
        return 1;
    }

    std::cout << "Parsing successful!" << std::endl;
    std::cout << program->to_string() << std::endl;

    return 0;
}
