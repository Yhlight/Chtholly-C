#include "Chtholly.h"
#include "Lexer.h"
#include "Parser.h"
#include "Transpiler.h"
#include "Error.h"
#include <iostream>
#include <fstream>
#include <sstream>

void Chtholly::runFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << path << std::endl;
        exit(74);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    Chtholly chtholly;
    chtholly.run(buffer.str());
    if (ErrorReporter::hadError) exit(65);
}

void Chtholly::runPrompt() {
    Chtholly chtholly;
    for (;;) {
        std::cout << "> ";
        std::string line;
        if (!std::getline(std::cin, line)) break;
        chtholly.run(line);
        ErrorReporter::hadError = false;
    }
}

std::vector<std::unique_ptr<Stmt>> Chtholly::run(const std::string& source, bool is_module) {
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto statements = parser.parse();

    if (ErrorReporter::hadError) return {};

    resolver.resolve(statements);

    if (ErrorReporter::hadError) return {};

    if (!is_module) {
        Transpiler transpiler;
        std::string cpp_code = transpiler.transpile(statements);

        std::ofstream out("out.cpp");
        out << cpp_code;
        out.close();

        int compile_result = system("g++ out.cpp -o out -std=c++17");
        if (compile_result != 0) {
            std::cerr << "Compilation failed." << std::endl;
            return {};
        }

        int run_result = system("./out");
        if (run_result != 0) {
            std::cerr << "Execution failed." << std::endl;
        }
    }

    return statements;
}
