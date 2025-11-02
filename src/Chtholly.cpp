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
    run(buffer.str());
    if (ErrorReporter::hadError) exit(65);
}

void Chtholly::runPrompt() {
    for (;;) {
        std::cout << "> ";
        std::string line;
        if (!std::getline(std::cin, line)) break;
        run(line);
        ErrorReporter::hadError = false;
    }
}

void Chtholly::run(const std::string& source) {
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto statements = parser.parse();

    if (ErrorReporter::hadError) return;

    Transpiler transpiler;
    std::string cppCode = transpiler.transpile(statements);
    std::cout << cppCode << std::endl;
}
