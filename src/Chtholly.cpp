#include "Chtholly.h"
#include "Lexer.h"
#include "PrattParser.h"
#include "Resolver.h"
#include "Transpiler.h"
#include "Error.h"
#include <iostream>
#include <fstream>
#include <sstream>

Chtholly::Chtholly() = default;

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
    if (ErrorReporter::hadRuntimeError) exit(70);
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
    if (ErrorReporter::hadError) return {};

    PrattParser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();
    if (ErrorReporter::hadError) return {};

    Resolver resolver;
    resolver.resolve(statements);
    if (ErrorReporter::hadError) return {};

    if (is_module) {
        return statements;
    }

    Transpiler transpiler;
    std::string output = transpiler.transpile(statements);
    if (ErrorReporter::hadError) return {};

    std::ofstream out("out.cpp");
    out << output;
    out.close();

    system("g++ out.cpp -o out");
    system("./out");

    return {};
}
