#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "Lexer.h"
#include "Parser.h"
#include "Resolver.h"
#include "Transpiler.h"

void run(const std::string& source) {
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    Resolver resolver;
    resolver.resolve(statements);

    Transpiler transpiler;
    std::string output = transpiler.transpile(statements);

    std::cout << output;
}

void runFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << path << std::endl;
        exit(1);
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
        // No file provided, we could run a REPL here in the future.
        std::cout << "Usage: chtholly [script]" << std::endl;
    }

    return 0;
}
