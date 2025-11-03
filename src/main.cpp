#include <iostream>
#include <fstream>
#include <vector>
#include "Lexer.h"
#include "Parser.h"
#include "SemanticAnalyzer.h"
#include "CodeGen.h"

void run(const std::string& source, const std::string& outputFile) {
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::vector<std::shared_ptr<Stmt>> statements = parser.parse();

    SemanticAnalyzer analyzer;
    analyzer.analyze(statements);

    const auto& errors = analyzer.getErrors();
    if (!errors.empty()) {
        for (const auto& error : errors) {
            std::cerr << error << std::endl;
        }
        return;
    }

    CodeGen generator;
    std::string cpp_code = generator.generate(statements);

    std::ofstream out(outputFile);
    out << cpp_code;
    out.close();
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <output_file>" << std::endl;
        return 1;
    }

    std::string source = R"(
        let a = 1;
        mut b = 2.5;
        b = a + b;
        print b;
        print "Hello, Chtholly!";
    )";

    run(source, argv[1]);

    return 0;
}
