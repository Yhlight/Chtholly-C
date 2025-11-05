#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "Lexer.h"
#include "Parser.h"
#include "Transpiler.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: chtholly [file]" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << argv[1] << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    try {
        chtholly::Lexer lexer(source);
        std::vector<chtholly::Token> tokens = lexer.scanTokens();

        chtholly::Parser parser(tokens);
        std::vector<std::unique_ptr<chtholly::Stmt>> statements = parser.parse();

        chtholly::Transpiler transpiler;
        std::string output = transpiler.transpile(statements);

        std::cout << output;
    } catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
