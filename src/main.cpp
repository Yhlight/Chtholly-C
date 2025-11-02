#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstdlib>
#include "../include/Lexer.h"
#include "../include/Parser.h"
#include "../include/CodeGen.h"

int main() {
    std::string source = R"(
        struct Test {
            private let name = "Chtholly";
            public let id = 114514;

            public func print_name() {
                print(name);
            }
        }

        let t = Test{};
        t.print_name();
    )";

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::shared_ptr<Stmt>> statements = parser.parse();

    CodeGen generator;
    std::string generated_code = generator.generate(statements);

    std::ofstream out("temp.cpp");
    out << generated_code;
    out.close();

    // The -w flag is added to suppress warnings that are not relevant to this test
    system("g++ -w temp.cpp -o temp && ./temp > output.txt");

    std::ifstream in("output.txt");
    std::string output((std::istreambuf_iterator<char>(in)),
                     std::istreambuf_iterator<char>());

    std::cout << output;

    // Clean up temporary files
    remove("temp.cpp");
    remove("temp");
    remove("output.txt");

    return 0;
}
