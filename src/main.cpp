#include <iostream>
#include <vector>
#include <string>
#include "../include/Lexer.h"
#include "../include/Parser.h"
#include "../include/CodeGen.h"

int main() {
    std::string source = R"(
        struct Point {
            let x = 0;
            let y = 0;
        }

        let p = Point{x: 10, y: 20};
        let x = p.x;
    )";

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::shared_ptr<Stmt>> statements = parser.parse();

    CodeGen generator;
    std::string generated_code = generator.generate(statements);

    std::cout << "// Generated C++ Code:\n";
    std::cout << "#include <iostream>\n\n";
    std::cout << generated_code;
    std::cout << "int main() {\n";
    std::cout << "std::cout << x << std::endl;\n";
    std::cout << "return 0;\n";
    std::cout << "}\n";


    return 0;
}
