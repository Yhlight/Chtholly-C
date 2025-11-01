#include "Chtholly.h"
#include "Lexer.h"
#include "Parser.h"
#include "Transpiler.h"
#include "Error.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

void Chtholly::runFile(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        std::cerr << "Could not open file: " << path << std::endl;
        exit(74);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    run(buffer.str());

    if (ErrorReporter::hadError) exit(65);
}

void Chtholly::runPrompt() {
    std::string line;
    for (;;) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break;
        run(line);
        ErrorReporter::reset();
    }
}

void Chtholly::run(const std::string& source) {
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    if(ErrorReporter::hadError) return;

    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    if(ErrorReporter::hadError) return;

    Transpiler transpiler;
    std::string output = transpiler.transpile(statements);

    std::cout << output << std::endl;
}
