#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "Lexer.h"
#include "Parser.h"
#include "SemanticAnalyzer.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <source_file> <output_file>" << std::endl;
        return 1;
    }

    std::ifstream sourceFile(argv[1]);
    if (!sourceFile.is_open()) {
        std::cerr << "Error: Could not open source file " << argv[1] << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << sourceFile.rdbuf();
    std::string sourceCode = buffer.str();

    Lexer lexer(sourceCode);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::shared_ptr<Stmt>> statements = parser.parse();

    SemanticAnalyzer analyzer;
    analyzer.analyze(statements);

    const auto& errors = analyzer.getErrors();
    if (!errors.empty()) {
        std::cerr << "Semantic errors found:" << std::endl;
        for (const auto& error : errors) {
            std::cerr << error << std::endl;
        }
        return 1;
    }

    std::cout << "No semantic errors found." << std::endl;

    return 0;
}
