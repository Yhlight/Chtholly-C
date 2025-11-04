#include "Chtholly.h"
#include "../frontend/Lexer.h"
#include "../frontend/Parser.h"
#include "../backend/Transpiler.h"
#include "../common/Error.h"
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
    std::string cpp_code = chtholly.run(buffer.str());
    if (ErrorReporter::hadError) exit(65);

    std::ofstream out("out.cpp");
    out << cpp_code;
    out.close();

    int compile_result = system("g++ out.cpp -o out -std=c++17");
    if (compile_result != 0) {
        std::cerr << "Compilation failed." << std::endl;
        exit(70);
    }

    int run_result = system("./out");
    if (run_result != 0) {
        std::cerr << "Execution failed." << std::endl;
        exit(71);
    }
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

std::string Chtholly::run(const std::string& source) {
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto statements = parser.parse();

    if (ErrorReporter::hadError) return "";

    resolver.resolve(statements);

    if (ErrorReporter::hadError) return "";

    Transpiler transpiler(resolver);
    return transpiler.transpile(statements);
}

std::vector<std::unique_ptr<Stmt>> Chtholly::run(const std::string& source, bool is_module) {
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto statements = parser.parse();

    if (ErrorReporter::hadError) return {};

    resolver.resolve(statements);

    if (ErrorReporter::hadError) return {};

    return statements;
}
