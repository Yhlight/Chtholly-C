#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <Lexer.h>
#include <Parser.h>
#include <ASTPrinter.h>
#include <SemanticAnalyzer.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <source_file>" << std::endl;
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

    // For now, we'll just print the AST if there are no errors.
    // In the future, we'll proceed to code generation.
    ASTPrinter printer;
    std::cout << printer.print(statements) << std::endl;


    return 0;
}
