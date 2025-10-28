#include <iostream>
#include <fstream>
#include <sstream>
#include "Lexer.h"
#include "Parser.h"
#include "AST/ASTPrinter.h"

void run(const std::string& source) {
    Chtholly::Lexer lexer(source);
    std::vector<Chtholly::Token> tokens = lexer.tokenize();
    Chtholly::Parser parser(tokens);
    std::vector<std::unique_ptr<Chtholly::Stmt>> statements = parser.parse();

    Chtholly::ASTPrinter printer;
    std::cout << printer.print(statements);
}

void runFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << path << std::endl;
        return;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    run(buffer.str());
}

int main(int argc, char* argv[]) {
    if (argc > 2) {
        std::cout << "Usage: chtholly [script]" << std::endl;
        return 64;
    } else if (argc == 2) {
        runFile(argv[1]);
    } else {
        std::cout << "Usage: chtholly [script]" << std::endl;
        return 64;
    }

    return 0;
}
