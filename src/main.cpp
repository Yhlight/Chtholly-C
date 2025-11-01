#include <iostream>
#include <vector>
#include <string>
#include "Lexer.h"
#include "Parser.h"
#include "CodeGen.h"
#include "AST.h"

int main() {
    std::string source = R"(
        mut a = 1;
        switch (a) {
            case 1: {
                a = 2;
                fallthrough;
            }
            case 2: {
                a = 3;
                break;
            }
            case 3: {
                a = 4;
            }
        }
    )";

    try {
        Lexer lexer(source);
        std::vector<Token> tokens = lexer.scanTokens();

        Parser parser(tokens);
        std::vector<std::shared_ptr<Stmt>> statements = parser.parse();

        if (!statements.empty()) {
            CodeGen codegen;
            std::string code = codegen.generate(statements);

            std::cout << "#include <iostream>\n\n";
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
