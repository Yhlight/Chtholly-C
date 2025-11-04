#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <Lexer.h>
#include <Parser.h>
#include <SemanticAnalyzer.h>
#include <CodeGen.h>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <source_file> <output_file>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Error: Could not open file " << argv[1] << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    Lexer lexer(source);
    std::vector<Token> tokens;
    Token token = lexer.nextToken();
    while (token.type != TokenType::END_OF_FILE) {
        tokens.push_back(token);
        token = lexer.nextToken();
    }
    tokens.push_back(token);

    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    SemanticAnalyzer analyzer;
    analyzer.analyze(statements);

    CodeGen codegen;
    std::string code = codegen.generate(statements);

    std::ofstream outFile(argv[2]);
    if (!outFile) {
        std::cerr << "Error: Could not open file " << argv[2] << std::endl;
        return 1;
    }

    outFile << code;

    return 0;
}
