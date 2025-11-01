#include <iostream>
#include <vector>
#include <string>
#include "Lexer.h"
#include "Parser.h"
#include "CodeGen.h"
#include "AST.h"

int main() {
    std::string source = R"(
        func print(i: int) {
            // This is a placeholder for a proper standard library function
        }
        struct Point {
            mut x: int;
            mut y: int;
        };
        let p = Point{x: 1, y: 2};
        print(p.x);
    )";

    try {
        Lexer lexer(source);
        std::vector<Token> tokens = lexer.scanTokens();

        Parser parser(tokens);
        std::vector<std::shared_ptr<Stmt>> statements = parser.parse();

        if (!statements.empty()) {
            CodeGen codegen;
            std::string code = codegen.generate(statements);

            std::cout << "#include <iostream>\n";
            std::cout << "#include <string>\n\n";
            std::cout << "int main() {\n";
            std::cout << code;
            std::cout << "    return 0;\n";
            std::cout << "}\n";
        }

    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
