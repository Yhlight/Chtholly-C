#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstdlib>
#include "../include/Lexer.h"
#include "../include/Parser.h"
#include "../include/CodeGen.h"
#include "../include/SemanticAnalyzer.h"

void run_test(const std::string& source, bool expect_error) {
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::shared_ptr<Stmt>> statements = parser.parse();

    SemanticAnalyzer analyzer;
    std::vector<std::string> errors = analyzer.analyze(statements);

    if (expect_error) {
        if (errors.empty()) {
            std::cout << "Expected semantic error, but none was found." << std::endl;
        } else {
            for (const auto& error : errors) {
                std::cout << error << std::endl;
            }
        }
    } else {
        if (!errors.empty()) {
            std::cout << "Unexpected semantic errors:" << std::endl;
            for (const auto& error : errors) {
                std::cout << error << std::endl;
            }
        } else {
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
        }
    }
}

int main() {
    std::cout << "--- Running Test 1: Expecting Semantic Error ---" << std::endl;
    run_test(R"(
        func consume(s: string) {
            print(s);
        }

        let s = "hello";
        consume(s);
        print(s);
    )", true);

    std::cout << "\n--- Running Test 2: Expecting Success ---" << std::endl;
    run_test(R"(
        struct Test {
            private let name = "Chtholly";
            public let id = 114514;

            public func print_name() {
                print(name);
            }
        }

        let t = Test{};
        t.print_name();
    )", false);

    return 0;
}
