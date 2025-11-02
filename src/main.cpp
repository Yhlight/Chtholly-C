#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstdlib>
#include <functional>
#include "../include/Lexer.h"
#include "../include/Parser.h"
#include "../include/CodeGen.h"
#include "../include/SemanticAnalyzer.h"

struct Test {
    std::string name;
    std::string source;
    bool expect_error;
    std::function<bool(const std::vector<std::string>&, const std::string&)> validate;
};

bool run_test(const Test& test) {
    std::cout << "--- Running Test: " << test.name << " ---" << std::endl;

    Lexer lexer(test.source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::shared_ptr<Stmt>> statements = parser.parse();

    SemanticAnalyzer analyzer;
    std::vector<std::string> errors = analyzer.analyze(statements);

    std::string output = "";

    if (test.expect_error) {
        if (errors.empty()) {
            std::cerr << "FAIL: Expected semantic error, but none was found." << std::endl;
            return false;
        }
    } else {
        if (!errors.empty()) {
            std::cerr << "FAIL: Unexpected semantic errors:" << std::endl;
            for (const auto& error : errors) {
                std::cerr << error << std::endl;
            }
            return false;
        }

        CodeGen generator;
        std::string generated_code = generator.generate(statements);

        std::ofstream out("temp.cpp");
        out << generated_code;
        out.close();

        // The -w flag is added to suppress warnings that are not relevant to this test
        system("g++ -w temp.cpp -o temp && ./temp > output.txt");

        std::ifstream in("output.txt");
        output.assign((std::istreambuf_iterator<char>(in)),
                       std::istreambuf_iterator<char>());

        // Clean up temporary files
        remove("temp.cpp");
        remove("temp");
        remove("output.txt");
    }

    if (test.validate && !test.validate(errors, output)) {
        std::cerr << "FAIL: Validation failed." << std::endl;
        return false;
    }

    std::cout << "PASS" << std::endl;
    return true;
}

int main() {
    std::vector<Test> tests = {
        {
            "Use after move",
            R"(
                func consume(s: string) {
                    print(s);
                }

                let s = "hello";
                consume(s);
                print(s);
            )",
            true,
            [](const std::vector<std::string>& errors, const std::string& output) {
                return !errors.empty() && errors[0].find("use of moved value 's'") != std::string::npos;
            }
        },
        {
            "Mutable borrow after immutable borrow",
            R"(
                let s = "hello";
                let r1 = &s;
                let r2 = &mut s;
            )",
            true,
            [](const std::vector<std::string>& errors, const std::string& output) {
                return !errors.empty() && errors[0].find("cannot mutably borrow") != std::string::npos;
            }
        },
        {
            "Immutable borrow after mutable borrow",
            R"(
                let s = "hello";
                let r1 = &mut s;
                let r2 = &s;
            )",
            true,
            [](const std::vector<std::string>& errors, const std::string& output) {
                return !errors.empty() && errors[0].find("cannot borrow") != std::string::npos;
            }
        },
        {
            "Valid nested borrows",
            R"(
                let s = "hello";
                let r1 = &s;
                {
                    let r2 = &s;
                    print(r2);
                }
                print(r1);
            )",
            false,
            [](const std::vector<std::string>& errors, const std::string& output) {
                return output == "hello\nhello\n";
            }
        },
        {
            "Binary expression with type inference",
            R"(
                let a = 10;
                let b = 20;
                let c = a + b;
                print(c);
            )",
            false,
            [](const std::vector<std::string>& errors, const std::string& output) {
                return output == "30\n";
            }
        }
    };

    int passed = 0;
    for (const auto& test : tests) {
        if (run_test(test)) {
            passed++;
        }
    }

    std::cout << "--- " << passed << "/" << tests.size() << " tests passed ---" << std::endl;

    return passed == tests.size() ? 0 : 1;
}
