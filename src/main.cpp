#include <iostream>
#include <vector>
#include <string>
#include "../include/Lexer.h"
#include "../include/Parser.h"
#include "../include/CodeGen.h"

int main() {
    std::string source = R"(
        func add(a, b) {
            return a + b;
        }

        let result = add(5, 10);
    )";

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::shared_ptr<Stmt>> statements = parser.parse();

    CodeGen generator;
    std::string generated_code = generator.generate(statements);

    std::cout << "// Generated C++ Code:\n";
    std::cout << "#include <iostream>\n\n";
    std::cout << "int main() {\n";
    std::cout << generated_code;
    std::cout << "std::cout << result << std::endl;\n";
    std::cout << "return 0;\n";
    std::cout << "}\n";


    return 0;
}
